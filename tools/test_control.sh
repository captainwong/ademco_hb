#!/bin/bash
# 端到端测试：httprelay 控制接口 <-> uvmachine 模拟器
# 依赖：python、curl、Git Bash；httprelay.exe（本仓库 x64 Release）、uvmachine.exe（libhb 私有仓库 vsprojects/Release）
# 维护者专用，不随 release 发布（uvmachine 为私有资产）
#
# 用法：bash tools/test_control.sh

set -e

RELAY_DIR="G:/dev_libs/ademco_hb/examples/x64/Release"
SIM_DIR="H:/dev_hb/libhb/vsprojects/Release"
TOOLS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ACCT=861234567890
TCP_PORT=12399
CB_PORT=9797
CTRL_PORT=8081
TOKEN=test123

CB_LOG=/tmp/cb.log
SIM_LOG=/tmp/sim.log
RELAY_LOG=/tmp/relay.log
SIMFIFO=/tmp/simcmd

PASS=0
FAIL=0

check() { # check <desc> <expr...>
    local desc=$1; shift
    if eval "$@"; then
        echo "PASS: $desc"
        PASS=$((PASS+1))
    else
        echo "FAIL: $desc"
        FAIL=$((FAIL+1))
    fi
}

post() { # post <json> [curl-args...]
    local json=$1; shift
    curl -s -X POST "http://127.0.0.1:${CTRL_PORT}/control" "$@" -H "Content-Type: application/json" -d "$json"
}

code_of() { grep -o '"code":[0-9]*' | grep -o '[0-9]*'; }

count_events() { # count_events <event> <zone> : 按事件码+zone 计数回调行
    grep -c "\"ademco_event\":$1,\"zone\":$2" "$CB_LOG" 2>/dev/null || true
}

assert_code() { # assert_code <desc> <expected> <response>
    local desc=$1 expected=$2 R=$3
    local got
    got=$(echo "$R" | code_of)
    if [ "$got" = "$expected" ]; then
        echo "PASS: $desc"
        PASS=$((PASS+1))
    else
        echo "FAIL: $desc (got=[$got] resp=[$R])"
        FAIL=$((FAIL+1))
    fi
}

# ---------- 清理残留 ----------
taskkill //IM httprelay.exe //F 2>/dev/null || true
taskkill //IM uvmachine.exe //F 2>/dev/null || true
rm -f "$CB_LOG" "$SIM_LOG" "$RELAY_LOG" "$SIMFIFO"

# ---------- 启动组件 ----------
python "$TOOLS_DIR/test_cb_server.py" "$CB_LOG" $CB_PORT &
CB_PID=$!

"$RELAY_DIR/httprelay.exe" $TCP_PORT "http://127.0.0.1:$CB_PORT/cb" \
    --control_port=$CTRL_PORT --token=$TOKEN > "$RELAY_LOG" 2>&1 &
RELAY_PID=$!

mkfifo "$SIMFIFO"
sleep 2147483 > "$SIMFIFO" &
FIFO_KEEP_PID=$!
"$SIM_DIR/uvmachine.exe" -s 127.0.0.1 -p $TCP_PORT -a $ACCT -i 1 --type 2 \
    < "$SIMFIFO" > "$SIM_LOG" 2>&1 &
SIM_PID=$!

sleep 3   # 等入网：type + status + ONLINE

echo "================ 1. 入网与转发 ================"
check "回调收到 ONLINE(1946)"            "grep -q '\"ademco_event\":1946' '$CB_LOG'"
check "回调收到主机类型事件"               "grep -qE '\"ademco_event\":17[0-9]{2}' '$CB_LOG'"
check "回调收到初始状态事件"               "grep -qE '\"ademco_event\":(3400|1400)' '$CB_LOG'"
check "relay 日志无错误"                  "[ -z \"\$(grep -i 'error' '$RELAY_LOG' | grep -v 'No error')\" ]"
check "sim 日志无 ACKERR"                 "! grep -q 'ACKERR' '$SIM_LOG'"

echo "================ 2. 控制接口 ================"
ARM0=$(count_events 3400 99)
R=$(post "{\"acct\":\"$ACCT\",\"ademco_event\":3400}" -H "Authorization: Bearer $TOKEN")
assert_code "布防(3400) code=0" 0 "$R"
sleep 1
ARM1=$(count_events 3400 99)
check "布防回报到达(3400 zone=99)"          "[ \"$ARM1\" -gt \"$ARM0\" ]"

R=$(post "{\"acct\":\"$ACCT\",\"ademco_event\":1400,\"password\":\"123456\"}" -H "Authorization: Bearer $TOKEN")
assert_code "撤防(1400, 正确密码) code=0" 0 "$R"
sleep 1
check "撤防回报到达(1400 zone=99)"          "grep -q '\"ademco_event\":1400,\"zone\":99' '$CB_LOG'"

R=$(post "{\"acct\":\"$ACCT\",\"ademco_event\":1400,\"password\":\"999999\"}" -H "Authorization: Bearer $TOKEN")
assert_code "撤防(错误密码) code=0 已下发" 0 "$R"
sleep 1
check "主机回报 1701(密码错误)"            "grep -q '\"ademco_event\":1701' '$CB_LOG'"

R=$(post "{\"acct\":\"$ACCT\",\"ademco_event\":1130}" -H "Authorization: Bearer $TOKEN")
assert_code "白名单外(1130) code=1" 1 "$R"

R=$(post "{\"acct\":\"$ACCT\",\"ademco_event\":1400}" -H "Authorization: Bearer $TOKEN")
assert_code "撤防缺密码 code=1" 1 "$R"

R=$(post "{\"acct\":\"$ACCT\",\"ademco_event\":1400,\"password\":\"123456\"}" -H "Authorization: Bearer wrong")
assert_code "错误 token code=2" 2 "$R"

R=$(post "{\"acct\":\"$ACCT\",\"ademco_event\":1400,\"password\":\"123456\"}")
assert_code "缺 token code=2" 2 "$R"

R=$(curl -s "http://127.0.0.1:$CTRL_PORT/control")
assert_code "GET 请求 code=1" 1 "$R"

R=$(post "{\"acct\":\"869999999999\",\"ademco_event\":3400}" -H "Authorization: Bearer $TOKEN")
assert_code "未知 acct code=3" 3 "$R"

echo "================ 3. 掉线与索引清理 ================"
kill $SIM_PID 2>/dev/null || true
sleep 2
check "回调收到 OFFLINE(1944)"             "grep -q '\"ademco_event\":1944' '$CB_LOG'"
R=$(post "{\"acct\":\"$ACCT\",\"ademco_event\":3400}" -H "Authorization: Bearer $TOKEN")
check "掉线后控制 code=3"                  "[ \"\$(echo \"\$R\" | code_of)\" = 3 ]"

echo "================ 清理 ================"
kill $RELAY_PID $CB_PID $FIFO_KEEP_PID 2>/dev/null || true
taskkill //IM httprelay.exe //F 2>/dev/null || true
rm -f "$SIMFIFO"

echo
echo "================ 结果：PASS=$PASS FAIL=$FAIL ================"
echo "日志: $CB_LOG $SIM_LOG $RELAY_LOG"
[ $FAIL -eq 0 ]
