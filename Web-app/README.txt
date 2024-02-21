UPDATE best_worst_table
SET current = (
    SELECT MAX(current)
    FROM last_year
    WHERE month = 10 AND year = 2023
    GROUP BY month, year
)
WHERE best_worst = 'worst' AND month = 10 AND year = 2023;

