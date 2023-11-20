const assert = require('assert');

const libname = process.platform === "win32" ? "libademco_js" : "ademco_js";
const libademco = require(`./build/Release/${libname}`);

libademco.ademcoPrint("hello nodejs", "hello nodejs".length);

console.log(libademco);

function test_parse() {
    console.log("test parse...");
    var str = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
    var cb = libademco.new_size_tp();
    var pkt = new libademco.AdemcoPacket();
    var res = libademco.ademcoPacketParse(str, str.length, pkt, cb, null);
    assert.strictEqual(res, libademco.RESULT_OK);
    assert.strictEqual(libademco.size_tp_value(cb), str.length);
    assert.strictEqual(pkt.crc, 0xC5C3);
    assert.strictEqual(pkt.len, 0x0053);
    assert.strictEqual(pkt.id, libademco.AID_HB);
    assert.strictEqual(pkt.seq, 0);
    assert.strictEqual(pkt.acct, "90219125916578");
    assert.strictEqual(pkt.data.ademcoId, 0);
    assert.strictEqual(pkt.data.ademcoEvent, libademco.EVENT_I_AM_WIRE_MACHINE);
    assert.strictEqual(pkt.data.gg, 0);
    assert.strictEqual(pkt.data.zone, 0);
    console.log("res=%d, commited=%d", res, libademco.size_tp_value(cb));
}

function test_pack() {
    console.log("test pack");
    var pkt = new libademco.AdemcoPacket();
    libademco.ademcoMakeHbPacket2(pkt, 1, "861234567890", 666666, libademco.EVENT_ARM, 0, 0, null);
    console.log("len=%d", pkt.raw_len);
    libademco.ademcoPrint(pkt.raw, pkt.raw_len);

    console.log("test parse packed data");
    var pkt2 = new libademco.AdemcoPacket();
    var cb = libademco.new_size_tp();
    var res = libademco.ademcoPacketParse(pkt.raw, pkt.raw_len, pkt2, cb, null);
    assert.strictEqual(res, libademco.RESULT_OK);
    assert.strictEqual(libademco.size_tp_value(cb), pkt.raw_len);
    assert.strictEqual(pkt2.id, libademco.AID_HB);
    assert.strictEqual(pkt2.seq, 1);
    assert.strictEqual(pkt2.acct, "861234567890");
    assert.strictEqual(pkt2.data.ademcoId, 666666);
    assert.strictEqual(pkt2.data.ademcoEvent, libademco.EVENT_ARM);
    assert.strictEqual(pkt2.data.gg, 0);
    assert.strictEqual(pkt2.data.zone, 0);
}

class AlarmHost {

    constructor(socket) {
        this.socket = socket;
        this.address = socket.address().address;
        this.port = socket.address().port;
        this.buf = null;
        this.inpkt = new libademco.AdemcoPacket();
        this.outpkt = new libademco.AdemcoPacket();
        this.parseErr = new libademco.AdemcoParseError();
        this.cb = libademco.new_size_tp();
        this.ademcoId = 0;
        this.acct = '';
        this.seq = 1;

        socket.on('data', (chunk) => {
            if (this.buf) {
                chunk = Buffer.concat([this.buf, chunk]);
            } 
            
            let res = libademco.ademcoPacketParse(chunk.toString(), chunk.length, this.inpkt, this.cb, this.parseErr);
            while (res === libademco.RESULT_OK) {
                chunk = chunk.slice(libademco.size_tp_value(this.cb));
                this.handleMsg();
                res = libademco.ademcoPacketParse(chunk.toString(), chunk.length, this.inpkt, this.cb, this.parseErr);
            }
    
            if (res === libademco.RESULT_ERROR) {
                console.log('parse error at line=%d, msg=%s', this.parseErr.line, this.parseErr.msg);
                chunk = null;
            } 
            
            this.buf = chunk;
        });
    
        socket.on('close', (error) => {
            console.log(this.tag() + ' disconnected, hadError=' + error);
        });

        console.log(this.tag() + ' connectd');
    }

    handleMsg() {
        console.log(this.tag() + ':');
        libademco.ademcoPrint(this.inpkt.raw, this.inpkt.raw_len);

        switch (this.inpkt.id) {
            case libademco.AID_NULL:
                this.acct = this.inpkt.acct;
                this.sendAck(this.inpkt.seq);
                break;
            
            case libademco.AID_HB:
            case libademco.AID_ADM_CID:
                this.acct = this.inpkt.acct;
                this.ademcoId = this.inpkt.data.ademcoId;
                this.sendAck(this.inpkt.seq);
                break;
            
        }

    }

    tag() {
        var str = this.address + ':' + this.port;
        if (this.acct) {
            str += ' acct=' + this.acct;
        }
        if (this.ademcoId) {
            str += ' ademco_id=' + this.ademcoId;
        }
        return str;
    }

    sendAck(seq) {
        libademco.ademcoMakeAckPacket2(this.outpkt, seq, this.acct, this.ademcoId);
        console.log('server reply:');
        libademco.ademcoPrint(this.outpkt.raw, this.outpkt.raw_len);
        this.socket.write(Buffer.from(this.outpkt.raw, this.outpkt.raw_len));
    }

}

function simpleServer(host, port) {
    const net = require('net');
    const server = net.createServer();

    server.on('error', (error) => {
        console.log(`server error: ${error}`); 
    });

    server.on('listening', () => {
        console.log(`server running at ${host}:${port}...`);
        
        server.on('connection', (socket) => {
            new AlarmHost(socket);
        });
    });

    
    server.listen(port, host);
}

test_parse();
test_pack();

simpleServer("0.0.0.0", 12345);
