// Assignment to a variable
// Function parameters are immutable by default in Zig,
// but idk if they are in our code
const ifj = @import("ifj24.zig");

pub fn help_me_god(a : i32) void {
    a = 2;
}

pub fn main () void {
    help_me_god(10);
}