PAPER_MUNCHER="ck run paper-muncher --release --props:async=epoll --"

echo "Should pass"
cat readme.md | $PAPER_MUNCHER --sandboxed fd:stdin.md -o fd:stdout.pdf > /dev/null
echo "Should fail"
cat readme.md | $PAPER_MUNCHER --sandboxed readme.md -o fd:stdout.pdf > /dev/null
