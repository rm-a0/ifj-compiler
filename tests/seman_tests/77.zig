// Error. Incorrect use of pseudovariable '_'
const ifj = @import("ifj24.zig");

pub fn main() void {
    _ = ifj.readi32();
    ifj.write(_); // Attempt to use '_', which should not be used
}
