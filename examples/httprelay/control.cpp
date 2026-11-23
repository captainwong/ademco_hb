/*
 * httprelay remote-control interface (C++, extern "C" to httprelay.c)
 *
 * POST /control  { "acct":"...", "ademco_event":1400, "gg":0, "zone":99,
 *                  "password":"123456" }
 * fire-and-forget: response only confirms the command was written to the
 * machine's tcp connection; execution result arrives as a regular status
 * event through the callback relay chain.
 * All responses are HTTP 200 with a body {"code":N,"msg":"..."}:
 *   0 ok, 1 bad request, 2 unauthorized, 3 machine offline
 */
#include "control.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>

#include <libhb/hb_core_ademco.h>
#include "cJSON/cJSON.h"
#include "uvlib/uv_httpd.h"
#include "uvlib/uv_log.h"
#include "uvlib/uv_tcpserver.h"

namespace {

/* control response codes, see file header */
enum class control_code_t {
    OK = 0,             /* command written to machine connection */
    BAD_REQUEST = 1,    /* invalid json / missing field / event not whitelisted / bad password */
    UNAUTHORIZED = 2,   /* token missing or wrong */
    MACHINE_OFFLINE = 3,/* acct has no live tcp connection */
};

struct machine_entry_t {
    uv_tcpserver_client_context_t* cli;
    ademco_id_t ademco_id;
};

/* acct -> machine connection */
std::unordered_map<std::string, machine_entry_t> g_index;

control_settings_t g_settings;

/* only these events may be injected (whitelist) */
const ademco_event_t CONTROL_EVENT_WHITELIST[] = {
    EVENT_DISARM,    /* 1400 */
    EVENT_ARM_AWAY,  /* 3400 */
    EVENT_ARM_STAY,  /* 3456 */
};

bool in_whitelist(ademco_event_t e) {
    for (ademco_event_t w : CONTROL_EVENT_WHITELIST) {
        if (e == w) return true;
    }
    return false;
}

void respond(uv_httpd_client_t* client, control_code_t code, const char* msg) {
    char resp[512];
    int body_len = snprintf(resp + 200, sizeof(resp) - 200, "{\"code\":%d,\"msg\":\"%s\"}", (int)code, msg);
    if (body_len < 0) body_len = 0;
    int total = snprintf(resp, 200,
                         "HTTP/1.1 200 OK\r\n"
                         "Content-Type: application/json\r\n"
                         "Content-Length: %d\r\n"
                         "Connection: close\r\n"
                         "\r\n",
                         body_len);
    memmove(resp + total, resp + 200, body_len);
    uv_httpd_write_response(client, resp, total + body_len);
}

bool str_ieq(const char* a, const char* b, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        if (tolower(ca) != tolower(cb)) return false;
        if (!ca) return true;
    }
    return true;
}

std::string header_value(const uv_httpd_request_t* req, const char* name) {
    size_t nlen = strlen(name);
    for (size_t i = 0; i < req->headers.n; ++i) {
        const uv_httpd_header_t& h = req->headers.headers[i];
        if (h.key.len == nlen && str_ieq(req->base + h.key.offset, name, nlen)) {
            return std::string(req->base + h.value.offset, h.value.len);
        }
    }
    return std::string();
}

/* constant-time-ish compare of "Bearer <token>" vs configured token */
bool token_ok(const uv_httpd_request_t* req) {
    if (g_settings.token[0] == '\0') return true; /* no auth configured */
    std::string auth = header_value(req, "Authorization");
    const char prefix[] = "Bearer ";
    if (auth.compare(0, sizeof(prefix) - 1, prefix) != 0) return false;
    const char* got = auth.c_str() + sizeof(prefix) - 1;
    const char* want = g_settings.token;
    unsigned diff = (unsigned)strlen(want) ^ (unsigned)strlen(got);
    for (size_t i = 0; i < strlen(want) || i < strlen(got); ++i) {
        unsigned char g = i < strlen(got) ? (unsigned char)got[i] : 0;
        unsigned char w = i < strlen(want) ? (unsigned char)want[i] : 0;
        diff |= (unsigned)(g ^ w);
    }
    return diff == 0;
}

/* build HENG-BO control packet and write to machine connection. */
control_code_t dispatch(const char* acct, ademco_event_t event, ademco_gg_t gg, ademco_zone_t zone, const char* password) {
    auto it = g_index.find(acct);
    if (it == g_index.end() || !it->second.cli) return control_code_t::MACHINE_OFFLINE;

    ademco_char_t buf[1024];
    size_t len = 0;

    if (event == EVENT_DISARM && password && password[0]) {
        ademco_xdata_t xdata;
        if (!ademco_make_xdata(&xdata, ADEMCO_XDATA_LENGTH_FMT_TWO_HEX,
                               ADEMCO_XDATA_TRANSFORM_AS_IS, password, strlen(password))) {
            return control_code_t::BAD_REQUEST;
        }
        len = ademco_make_hb_packet(buf, sizeof(buf), 1, acct, it->second.ademco_id,
                                    event, gg, zone, &xdata);
    } else {
        len = ademco_make_hb_packet(buf, sizeof(buf), 1, acct, it->second.ademco_id,
                                    event, gg, zone, NULL);
    }

    if (len == 0) return control_code_t::BAD_REQUEST;

    printf("C(send control): acct=%s event=%d gg=%d zone=%d\n", acct, (int)event, (int)gg, (int)zone);
    uv_tcpserver_send_to_cli(it->second.cli, buf, len);
    printf("S:");
    ademco_print(buf, len);
    return control_code_t::OK;
}

void on_request(uv_httpd_server_t* server, uv_httpd_client_t* client, uv_httpd_request_t* req) {
    (void)server;

    if (req->method != HTTP_POST) {
        respond(client, control_code_t::BAD_REQUEST, "bad request: POST /control required");
        return;
    }

    /* url may carry query string, compare path only */
    size_t url_len = req->url.len;
    const char* url = req->base + req->url.offset;
    for (size_t i = 0; i < url_len; ++i) {
        if (url[i] == '?') {
            url_len = i;
            break;
        }
    }
    if (url_len != 8 || strncmp(url, "/control", 8) != 0) {
        respond(client, control_code_t::BAD_REQUEST, "bad request: POST /control required");
        return;
    }

    if (!token_ok(req)) {
        respond(client, control_code_t::UNAUTHORIZED, "unauthorized");
        return;
    }

    std::string body(req->base + req->body.offset, req->body.len);
    cJSON* json = cJSON_Parse(body.c_str());
    if (!json) {
        respond(client, control_code_t::BAD_REQUEST, "bad request: invalid json");
        return;
    }

    const cJSON* j_acct = cJSON_GetObjectItemCaseSensitive(json, "acct");
    const cJSON* j_event = cJSON_GetObjectItemCaseSensitive(json, "ademco_event");
    const cJSON* j_gg = cJSON_GetObjectItemCaseSensitive(json, "gg");
    const cJSON* j_zone = cJSON_GetObjectItemCaseSensitive(json, "zone");
    const cJSON* j_pwd = cJSON_GetObjectItemCaseSensitive(json, "password");

    control_code_t rc = control_code_t::BAD_REQUEST;
    const char* msg = "bad request";

    do {
        if (!cJSON_IsString(j_acct) || j_acct->valuestring[0] == '\0') {
            msg = "bad request: acct required";
            break;
        }
        if (!cJSON_IsNumber(j_event)) {
            msg = "bad request: ademco_event required";
            break;
        }

        ademco_event_t event = (ademco_event_t)(int)j_event->valuedouble;
        ademco_gg_t gg = cJSON_IsNumber(j_gg) ? (ademco_gg_t)(int)j_gg->valuedouble : 0;
        ademco_zone_t zone = cJSON_IsNumber(j_zone) ? (ademco_zone_t)(int)j_zone->valuedouble : 99;
        const char* password = cJSON_IsString(j_pwd) ? j_pwd->valuestring : NULL;

        if (!in_whitelist(event)) {
            msg = "bad request: ademco_event not allowed";
            break;
        }
        if (event == EVENT_DISARM) {
            if (!password || !ademco_is_valid_password(password)) {
                msg = "bad request: valid password required for disarm";
                break;
            }
        }

        rc = dispatch(j_acct->valuestring, event, gg, zone, password);
        msg = (rc == control_code_t::OK) ? "ok" : "machine offline";
    } while (0);

    cJSON_Delete(json);
    respond(client, rc, msg);
}

} /* namespace */

extern "C" {

int control_parse_args(int argc, char** argv, control_settings_t* settings, const char** positional) {
    memset(settings, 0, sizeof(*settings));
    strcpy(settings->listen_addr, "127.0.0.1");
    int npos = 0;

    for (int i = 1; i < argc; ++i) {
        const char* a = argv[i];
        if (strncmp(a, "--", 2) != 0) {
            if (npos >= 2) return -1; /* too many positional args */
            positional[npos++] = a;
            continue;
        }

        const char* eq = strchr(a + 2, '=');
        std::string key = eq ? std::string(a + 2, eq - (a + 2)) : std::string(a + 2);
        std::string val;
        bool has_val = false;
        if (eq) {
            val = eq + 1;
            has_val = true;
        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
            val = argv[i + 1];
            has_val = true;
            ++i;
        }
        if (!has_val) return -1;

        if (key == "control_port") {
            settings->port = atoi(val.c_str());
            settings->enabled = settings->port > 0;
        } else if (key == "listen_addr") {
            if (val.size() >= sizeof(settings->listen_addr)) return -1;
            strcpy(settings->listen_addr, val.c_str());
        } else if (key == "token") {
            if (val.size() >= sizeof(settings->token)) return -1;
            strcpy(settings->token, val.c_str());
        } else {
            return -1; /* unknown option */
        }
    }

    return npos;
}

int control_start(const control_settings_t* settings) {
    if (!settings->enabled) return 0;
    g_settings = *settings;

    uv_httpd_server_t* httpd = NULL;
    int r = uv_httpd_create(&httpd, uv_default_loop(), on_request);
    if (r) return r;
    r = uv_httpd_listen(httpd, settings->listen_addr, settings->port);
    if (r) return r;

    printf("control server listening on %s:%d%s\n",
           settings->listen_addr, settings->port,
           settings->token[0] ? " (token enabled)" : " (WARNING: no token, unauthenticated)");
    return 0;
}

void control_index_update(const char* acct, void* cli, unsigned long ademco_id) {
    if (!acct || !acct[0]) return;
    machine_entry_t& e = g_index[acct];
    e.cli = (uv_tcpserver_client_context_t*)cli;
    if (ademco_id) e.ademco_id = (ademco_id_t)ademco_id;
}

void control_index_remove(const char* acct) {
    if (!acct || !acct[0]) return;
    g_index.erase(acct);
}

} /* extern "C" */
