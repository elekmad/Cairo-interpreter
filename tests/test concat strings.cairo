a=["ga", "bu", "zo"];
b="foo";
c="bar";

message("a, b", a << b);
message("b, a", b << a);
message("b, c", b << c);
message("b, a, c", b << a << c);