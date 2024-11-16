// Test for redefiniton of function (ifj.string != string)
const ifj = @import("ifj24.zig");

pub fn string() void {
    var x = 4;
}

pub fn main() void {
    string();
    ifj.string("ahoj");
}