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
virtual z :: Int
-- It is strongly recommended to have a virtual flag for virtual variables.
-- Then by using -virtualStrCheck option, we can eliminate all virtual variable definitions without explicit mention.

-- Virtual Type
V :: *
-- virtual type with virtual keyword
virtual W :: *
-- The two types W1 and W2 are equivalent (role of Maybe Int)
W1 :: *
W1 = Nothing | Just Int
data W2 = Nothing | Just Int
-- L1 and L2 are equivalent
L1 :: * -> *
L1 a = Empty | Cons a (L1 a)
data L2 a = Empty | Cons a (L2 a)

virtual Prop :: #
type ~Neg = Neg -> Bot

