// 'count' in while loop must be of nullable type
const ifj = @import("ifj24.zig");

pub fn main() void {
    var count: i32 = 5;
    count = count - 1;
    while (count) |value| {
        ifj.write(value);
        count = count - 1;
    }
}
