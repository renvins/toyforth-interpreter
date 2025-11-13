#!/bin/bash

# ToyForth Test Runner
# Runs all test files and verifies output matches expected results

set -e

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Build the project if needed
if [ ! -f "./toyforth" ]; then
    echo "Building toyforth..."
    make
fi

# Counter for results
PASSED=0
FAILED=0
TOTAL=0

echo "========================================"
echo "  ToyForth Test Suite"
echo "========================================"
echo ""

# Run each test file
for test_file in tests/*.tf; do
    # Skip test.tf (original test file, no expected output)
    if [ "$(basename "$test_file")" = "test.tf" ]; then
        continue
    fi
    
    TOTAL=$((TOTAL + 1))
    test_name=$(basename "$test_file" .tf)
    expected_file="tests/${test_name}.expected"
    
    # Check if expected file exists
    if [ ! -f "$expected_file" ]; then
        echo -e "${YELLOW}⊘ SKIP${NC} $test_name (no expected output file)"
        continue
    fi
    
    # Run the test and capture output
    actual_output=$(./toyforth "$test_file" 2>&1)
    expected_output=$(cat "$expected_file")
    
    # Compare outputs
    if [ "$actual_output" = "$expected_output" ]; then
        echo -e "${GREEN}✓ PASS${NC} $test_name"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ FAIL${NC} $test_name"
        echo "  Expected:"
        echo "$expected_output" | sed 's/^/    /'
        echo "  Got:"
        echo "$actual_output" | sed 's/^/    /'
        FAILED=$((FAILED + 1))
    fi
done

echo ""
echo "========================================"
echo "  Test Results"
echo "========================================"
echo "Total:  $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}Failed: $FAILED${NC}"
else
    echo -e "Failed: $FAILED"
fi
echo ""

# Exit with error code if any tests failed
if [ $FAILED -gt 0 ]; then
    exit 1
fi

echo -e "${GREEN}All tests passed! 🎉${NC}"
exit 0

