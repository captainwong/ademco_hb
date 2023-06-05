const assert = require('assert');
// windows:
const libademco = require('./build/Release/libademco_js');
// linux:
// const libademco = require('./build/Release/ademco_js');

libademco.ademcoPrint("hello nodejs", "hello nodejs".length);

console.log(libademco);

console.log("test parse...");
var str = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
var cb = libademco.new_size_tp();
var pkt = new libademco.AdemcoPacket();
var res = libademco.ademcoPacketParse(str, str.length, pkt, cb);
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