#!/usr/bin/env bash
# Codexion test runner. Run from the project root (where ./codexion is).
#   bash run_tests.sh            -> functional tests (+ checker if python3 exists)
#   bash run_tests.sh --tools    -> also helgrind / drd / memcheck / TSan (Linux only)
# Every test writes its output to tests_out/<name>.txt
# and the checker verdict to tests_out/<name>.check.txt
# Final summary: tests_out/SUMMARY.txt

BIN=./codexion
OUT=tests_out
CHECK="$(dirname "$0")/check_log.py"
PY=$(command -v python3 || command -v python)
mkdir -p "$OUT"
: > "$OUT/SUMMARY.txt"

[ -x "$BIN" ] || { echo "Run 'make' first ($BIN not found)"; exit 1; }

say() { echo "$*" | tee -a "$OUT/SUMMARY.txt"; }

# functional <name> <expect: burnout|noburnout|any> <args...>
functional() {
	local name=$1 expect=$2; shift 2
	local start end rc
	start=$(date +%s%3N)
	timeout 120 $BIN "$@" > "$OUT/$name.txt" 2> "$OUT/$name.err.txt"
	rc=$?
	end=$(date +%s%3N)
	local verdict="(no python: check $OUT/$name.txt by hand)"
	if [ -n "$PY" ] && [ -f "$CHECK" ]; then
		$PY "$CHECK" "$OUT/$name.txt" "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$expect" \
			> "$OUT/$name.check.txt" 2>&1
		verdict=$(grep RESULT "$OUT/$name.check.txt")
	fi
	[ $rc -eq 124 ] && verdict="TIMEOUT (program never stopped) / $verdict"
	say "$(printf '%-28s exit=%-3s %5sms  %s' "$name" "$rc" "$((end-start))" "$verdict")"
}

say "=== FUNCTIONAL (subject / evaluation sheet) ==="
functional 01_one_coder_burnout    burnout   1 800 200 200 200 10 0 fifo
functional 02_fifo_noburnout       noburnout 5 2000 200 200 200 10 0 fifo
functional 03_edf_noburnout        noburnout 5 2000 200 200 200 7 0 edf
functional 04_cooldown400_fifo     noburnout 5 3000 200 200 200 10 400 fifo
functional 05a_cooldown800_fifo    any       5 3000 200 200 200 10 800 fifo
functional 05b_cooldown800_edf     any       5 3000 200 200 200 10 800 edf
functional 06_burnout_by_design    burnout   5 500 200 200 200 10 0 fifo

say ""
say "=== EXTRA (odd/even N, larger N, tight timings) ==="
functional 10_two_coders           noburnout 2 1500 200 200 200 6 0 fifo
functional 11_three_coders_edf     noburnout 3 2000 200 200 200 6 0 edf
functional 12_four_coders_fifo     noburnout 4 2000 200 200 200 6 0 fifo
functional 13_seven_coders_edf     noburnout 7 3000 200 150 150 5 0 edf
functional 14_cooldown100_edf      noburnout 5 2000 200 200 200 6 100 edf
functional 15_200_coders           noburnout 200 3000 100 100 100 3 0 edf
functional 16_three_coders_tight   noburnout 3 450 100 100 100 5 0 fifo
functional 18_burnout_3_coders     burnout   3 250 100 100 100 5 0 fifo
functional 17_burnout_2_coders     burnout   2 250 100 100 100 5 0 edf

say ""
say "=== ARGUMENT VALIDATION (each must print an error and exit != 0, no output on stdout) ==="
bad() {
	local name=$1; shift
	$BIN "$@" > "$OUT/$name.txt" 2> "$OUT/$name.err.txt"
	local rc=$?
	local lines
	lines=$(wc -l < "$OUT/$name.txt")
	if [ $rc -ne 0 ] && [ "$lines" -eq 0 ]; then r=PASS; else r=FAIL; fi
	say "$(printf '%-28s exit=%-3s stdout_lines=%-3s %s  (%s)' "$name" "$rc" "$lines" "$r" "$(head -c 60 "$OUT/$name.err.txt" | tr '\n' ' ')")"
}
bad 20_args_missing       5 800 200 200 200 3 0
bad 21_args_extra         5 800 200 200 200 3 0 fifo x
bad 22_negative           5 800 200 200 200 3 -1 fifo
bad 23_not_integer        5 800 abc 200 200 3 0 fifo
bad 24_float              5 800 20.5 200 200 3 0 fifo
bad 25_scheduler_lifo     5 800 200 200 200 3 0 lifo
bad 26_scheduler_upper    5 800 200 200 200 3 0 FIFO
bad 27_zero_coders        0 800 200 200 200 3 0 fifo
bad 28_zero_required      5 800 200 200 200 0 0 fifo
bad 29_empty_arg          5 800 "" 200 200 3 0 fifo

if [ "$1" = "--tools" ]; then
	say ""
	say "=== CONCURRENCY TOOLS (Linux/WSL only) ==="
	tool() {
		local name=$1 cmd=$2; shift 2
		timeout 300 $cmd $BIN "$@" > /dev/null 2> "$OUT/$name.txt"
		local rc=$?
		local summ
		summ=$(grep -E "ERROR SUMMARY" "$OUT/$name.txt" | tail -1)
		say "$(printf '%-28s exit=%-3s %s' "$name" "$rc" "$summ")"
	}
	if command -v valgrind > /dev/null; then
		for t in "5 2000 200 200 200 3 0 fifo" "5 3000 200 200 200 4 400 edf" \
			"5 500 200 200 200 10 0 fifo" "1 800 200 200 200 10 0 fifo"; do
			tag=$(echo "$t" | tr ' ' '_')
			tool "helgrind_$tag" "valgrind --tool=helgrind --error-exitcode=9" $t
			tool "drd_$tag" "valgrind --tool=drd --error-exitcode=9" $t
			tool "memcheck_$tag" "valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=9" $t
		done
	else
		say "valgrind not installed (sudo apt install valgrind)"
	fi
	if cc -pthread -fsanitize=thread -g -I includes src/*.c -o "$OUT/codexion_tsan" 2> "$OUT/tsan_build.txt"; then
		for t in "5 2000 200 200 200 3 0 fifo" "5 3000 200 200 200 4 400 edf" \
			"5 500 200 200 200 10 0 fifo" "1 800 200 200 200 10 0 fifo"; do
			tag=$(echo "$t" | tr ' ' '_')
			timeout 120 "$OUT/codexion_tsan" $t > /dev/null 2> "$OUT/tsan_$tag.txt"
			rc=$?
			w=$(grep -c "WARNING: ThreadSanitizer" "$OUT/tsan_$tag.txt")
			say "$(printf '%-28s exit=%-3s tsan_warnings=%s' "tsan_$tag" "$rc" "$w")"
		done
	else
		say "TSan build failed (see $OUT/tsan_build.txt) - normal on Windows/MSYS2"
	fi
fi

say ""
say "Done. Open $OUT/SUMMARY.txt and any $OUT/<test>.check.txt for details."
