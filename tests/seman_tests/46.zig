// Cannot bind 'null' to variable 'value' of non-nullable type
const ifj = @import("ifj24.zig");

pub fn main() void {
    if (null) |value| { // Error
        ifj.write(value);
    } else {
        ifj.write("Value is null");
    }
}
