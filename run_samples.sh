#!/bin/bash
# Run all sample programs through meo compiler

MEO="./build/meo"
SAMPLE_DIR="sample"
PASS=0
FAIL=0
TOTAL=0

if [ ! -f "$MEO" ]; then
    echo "Build meo first: cmake -S . -B build && cmake --build build"
    exit 1
fi

for src in "$SAMPLE_DIR"/*.c; do
    name=$(basename "$src" .c)
    TOTAL=$((TOTAL + 1))
    printf "%-20s " "$name"

    if $MEO "$src" 2>/dev/null; then
        if [ ! -f a.out ]; then
            cc -no-pie -o a.out out.s 2>/dev/null
        fi
        if [ -f a.out ]; then
            ./a.out
            code=$?
            echo "  (exit: $code)"
            PASS=$((PASS + 1))
            rm -f a.out out.s
        else
            echo "  FAIL (link error)"
            FAIL=$((FAIL + 1))
            rm -f out.s
        fi
    else
        echo "  FAIL (compile error)"
        FAIL=$((FAIL + 1))
        rm -f out.s
    fi
done

echo ""
echo "Results: $PASS/$TOTAL passed, $FAIL failed"
