#include <stdio.h>
#include "../../hb_com.h"
#include "uvlib/uv_log.h"
#include "uvlib/uv_tcpserver.h"
#include "cJSON/cJSON.h"
#include <curl/curl.h>
#include <string.h>

typedef struct machine_info_s {
	char acct[ADEMCO_PACKET_ACCT_MAX_LEN + 1];
	AdemcoId ademcoId;
	HbMachineType type;
	HbMachineStatus status;
}machine_info_t;

struct {
	uv_loop_t* loop;
	uv_tcpserver_t* tcpd;
	const char* uri;
}context;

size_t on_write(void* buffer, size_t size, size_t nmemb, void* user) {
	mybuf_t* buf = user;
	mybuf_append(buf, buffer, size * nmemb);
	return size * nmemb;
}

int post(const char* json) {
	CURLcode res;
	CURL* curl = curl_easy_init();
	struct curl_slist* headers = NULL;
	mybuf_t resp;
	long http_status, header_size;

	mybuf_init(&resp);

	do {
		if (!curl) {
			res = CURLE_FAILED_INIT;
			uvlog_error("curl_easy_init failed");
			break;
		}

		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, on_write);
		curl_easy_setopt(curl, CURLOPT_URL, context.uri);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(json));
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);

		res = curl_easy_perform(curl);
		if (res == CURLE_OK || res == CURLE_GOT_NOTHING) {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
			curl_easy_getinfo(curl, CURLINFO_HEADER_SIZE, &header_size);
		}
		mybuf_reserve(&resp, resp.size + 1);
		resp.buf[resp.size] = '\0';

		uvlog_debug("RESP: %d %s\n"
					"%s\n",
					res, curl_easy_strerror(res),
					resp.buf);		

	} while (0);

	if (curl)
		curl_easy_cleanup(curl);

	if (headers) {
		curl_slist_free_all(headers);
		headers = NULL;
	}
	return 0;
}

int relay(const char* acct, AdemcoEvent ademco_event, AdemcoZone zone, AdemcoGG gg) {
	cJSON* json = NULL;
	char* string = NULL;
	int r = 0;

	json = cJSON_CreateObject();
	fatal_if_null(json);
	if (cJSON_AddStringToObject(json, "acct", acct) == NULL) goto end;
	if (cJSON_AddNumberToObject(json, "ademco_event", ademco_event) == NULL) goto end;
	if (cJSON_AddNumberToObject(json, "zone", zone) == NULL) goto end;
	if (cJSON_AddNumberToObject(json, "gg", gg) == NULL) goto end;
	string = cJSON_Print(json);
	if (string == NULL) goto end;

	uvlog_debug("OUT:\n%s\n", string);
	r = post(string);

end:
	if (json)
		cJSON_Delete(json);
	if (string)
		free(string);
	return r;
}

void on_tcp_connection(uv_tcpserver_client_context_t* client, int connected) {
	if (connected) {
		machine_info_t* machine = malloc(sizeof(machine_info_t));
		memset(machine->acct, '\0', sizeof(machine->acct));
		machine->ademcoId = 0;
		machine->type = HMT_INVALID;
		machine->status = HMS_INVALID;
		client->data = machine;
	} else if(client->data) {
		if (((machine_info_t*)(client->data))->acct[0] != '\0') {
			relay(((machine_info_t*)(client->data))->acct, EVENT_OFFLINE, 0, 0);
		}
		free(client->data);
	}
}

uv_tcp_parse_result_t on_tcp_parse(uv_tcpserver_client_context_t* client, const char* buf, size_t len, size_t* ate) {
	AdemcoPacket pkt;
	AdemcoParseResult res = ademcoPacketParse(buf, len, &pkt, ate);
	switch (res) {
	case RESULT_OK:
		switch (pkt.id) {
		case AID_NULL:			
		case AID_HB:
		case AID_ADM_CID:
			printf("C:");
			ademcoPrint(pkt.raw, pkt.raw_len);

			if (((machine_info_t*)(client->data))->acct[0] == '\0') {
				strcpy(((machine_info_t*)(client->data))->acct, pkt.acct);
				relay(pkt.acct, EVENT_ONLINE, 0, 0);
			}

			if (pkt.data.ademcoId) {
				((machine_info_t*)(client->data))->ademcoId = pkt.data.ademcoId;
			}

			if (ademcoIsMachineStatusEvent(pkt.data.ademcoEvent)) {
				((machine_info_t*)(client->data))->status = hbMachineStatusFromAdemcoEvent(pkt.data.ademcoEvent);
			} 
			
			if (ademcoIsMachineTypeEvent(pkt.data.ademcoEvent)) {
				((machine_info_t*)(client->data))->type = hbMachineTypeFromAdemcoEvent(pkt.data.ademcoEvent);
			} 
			
			if (pkt.data.ademcoEvent != EVENT_INVALID_EVENT && ((machine_info_t*)(client->data))->acct[0] != '\0') {
				relay(((machine_info_t*)(client->data))->acct, pkt.data.ademcoEvent, pkt.data.zone, pkt.data.gg);
			}

			ademcoMakeAckPacket2(&pkt, pkt.seq, pkt.acct, pkt.data.ademcoId);
			uv_tcpserver_send_to_cli(client, pkt.raw, pkt.raw_len);
			printf("S:");
			ademcoPrint(pkt.raw, pkt.raw_len);

			break;
		}
		return uv_tcp_parse_ok;
		break;
	case RESULT_NOT_ENOUGH:
		return uv_tcp_parse_not_enough;
		break;
	case RESULT_ERROR:
		return uv_tcp_parse_error;
		break;
	default:
		abort();
		break;
	}
}

int init_tcpd(int port) {
	static uv_tcpserver_settings_t settings = { on_tcp_connection, on_tcp_parse, NULL };
	int r = uv_tcpserver_create(&context.tcpd, context.loop, &settings);
	fatal_on_uv_err(r, "uv_tcpserver_create");
	r = uv_tcpserver_start_listen(context.tcpd, "0.0.0.0", port);
	fatal_on_uv_err(r, "uv_tcpserver_start_listen");
	printf("tcp server listening on %s:%d\n", "0.0.0.0", port);
	return r;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s tcp_server_listening_port http_client_relay_to_uri\n"
				"	Example: %s 12345 http://your-http-server.com:8080/ademco\n",
				argv[0], argv[0]);
		exit(1);
	}

	uv_log_set_level(uv_log_level_debug);
	memset(&context, 0, sizeof(context));
	context.uri = argv[2];

	context.loop = uv_default_loop();
	fatal_if_null(context.loop);

	curl_global_init(CURL_GLOBAL_ALL);

	if (init_tcpd(atoi(argv[1]))) {
		abort();
	}

	uv_run(context.loop, UV_RUN_DEFAULT);
}
