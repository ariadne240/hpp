-- Virtual Definition
v :: Int
-- no problem until the variable is explicitly used in main.
-- If main does not have a dependency on the variable,
-- it remains safe to leave it virtual.
w :: Int
w = v + 2
-- w is not strongly virtual.
-- We do not say that w is virtual (virtual == strongly virtual).
-- But since w has a dependency on the strongly virtual v,
-- w is indeed weakly virtual.
x :: Int
x = w + 2
-- x is weakly virtual but not virtual.
-- It does have a dependency on the strongly virtual v.
-- But it does not have a direct dependency on v.
-- It has a direct dependency on w.
y :: Int
y = x + v
-- y is weakly virtual but not virtual.
-- It does have a direct dependency on x or v.
-- It does not have a direct dependency on w.
-- It has a dependency on w.

-- Virtual Type

