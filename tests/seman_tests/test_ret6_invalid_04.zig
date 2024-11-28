// No return statement in non-void function
const ifj = @import("ifj24.zig");

pub fn help() i32 {
    if (1 < 2) {
        if (1 < 2) {
            ifj.readf64();
        }
    }

    return 3;
}

pub fn main() void {

}