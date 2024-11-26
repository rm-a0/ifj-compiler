// Error. Operator '==' cannot be applied to string literals. 
const ifj = @import("ifj24.zig");

pub fn main() void {
    if ("hello" == "world") {
        // This should not compile
    }
}