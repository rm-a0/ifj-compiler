// Error. Cannot perform implicit conversion on variable 'a' of type 'i32'.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var a: i32 = 10;
    var b: f64 = 20.0;
    var result: f64 = a + b;
}
