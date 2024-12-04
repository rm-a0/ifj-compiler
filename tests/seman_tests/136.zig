const ifj = @import("ifj24.zig");

pub fn main() void {
    const n = ifj.readi32();
    var a: i32 = 0;
    var b: i32 = 1;
    var i: i32 = 2;
    ifj.write("Fibonacciho cisla: ");
    ifj.write(a);
    ifj.write(", ");
    ifj.write(b);
    ifj.write(", ");
    var c: i32 = 0;
    while (i < n) {
        c = a + b;
        ifj.write(c);
        ifj.write(", ");
        a = b;
        b = c;
        i = i + 1;
    }
    ifj.write("\n");
}
