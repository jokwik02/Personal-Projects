#!/bin/bash
# Wrapper script to benchmark othellostart.sh
# Runs each time limit 5 times and counts wins for White and Black
# Usage: ./benchmark_othello.sh white_player black_player

if [ $# -lt 2 ]; then
    echo "Usage: $0 white_player black_player"
    exit 1
fi

WHITE="$1"
BLACK="$2"

echo "Time | White Wins | Black Wins"

for TIME in $(seq 2 10); do
    WHITE_WINS=0
    BLACK_WINS=0

    for RUN in $(seq 1 5); do
        # Run othellostart.sh and capture the last few lines
        OUTPUT=$(bash othellostart.sh "$WHITE" "$BLACK" "$TIME")

        # Extract winner from the output
        WINNER=$(echo "$OUTPUT" | grep "won with" | awk '{print $1}')

        if [ "$WINNER" == "White" ]; then
            WHITE_WINS=$((WHITE_WINS + 1))
        elif [ "$WINNER" == "Black" ]; then
            BLACK_WINS=$((BLACK_WINS + 1))
        fi
    done

    echo "$TIME     | $WHITE_WINS         | $BLACK_WINS"
done