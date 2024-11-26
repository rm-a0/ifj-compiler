// No Error
const ifj = @import("ifj24.zig");

fn getNullableNumber(a: i32, b: i32) ?i32 {
    if (a > b) {
        return a - b;
    } else {
        return null;
    }
}

pub fn main() void {
    if (getNullableNumber(5, 3)) |result| {
        ifj.write("Result: ");
        ifj.write(result);
    } else {
        ifj.write("No result.");
    }
}
