#!/bin/bash

# 설정
TINY_PORT=8080
PROXY_PORT=8000
FILES="home.html csapp.c tiny.c godzilla.jpg tiny"

# 디렉토리 생성
mkdir -p test_proxy test_direct

echo "=== Proxy 테스트 시작 ==="
echo ""

passed=0
total=0

for file in $FILES; do
    total=$((total + 1))
    echo "[$total] 테스트: $file"

    # 프록시를 통해 다운로드
    echo "  - 프록시로 다운로드..."
    curl -s --max-time 5 --proxy http://localhost:${PROXY_PORT} \
         http://localhost:${TINY_PORT}/${file} > test_proxy/${file}

    # 직접 다운로드
    echo "  - 직접 다운로드..."
    curl -s --max-time 5 http://localhost:${TINY_PORT}/${file} > test_direct/${file}

    # 비교
    if diff -q test_proxy/${file} test_direct/${file} > /dev/null 2>&1; then
        echo "  ✓ PASS - 파일이 동일합니다"
        passed=$((passed + 1))
    else
        echo "  ✗ FAIL - 파일이 다릅니다"
        echo "    파일 크기:"
        ls -lh test_proxy/${file} test_direct/${file} | awk '{print "    "$9": "$5}'
    fi
    echo ""
done

echo "==================================="
echo "결과: $passed/$total 성공"
echo "==================================="
