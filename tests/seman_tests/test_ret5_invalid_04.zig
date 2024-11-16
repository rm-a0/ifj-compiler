// Assignment to a variable
// Function parameters are immutable by default in Zig,
// but idk if they are in our code
const ifj = @import("ifj24.zig");

pub fn main (a : i32) void {
    a = 2;
}