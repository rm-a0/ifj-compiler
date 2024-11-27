// No return statement in non-void function
const ifj = @import("ifj24.zig");

pub fn help() i32 {
    if (1 < 2) {
        if (1 < 2) {
            const x = ifj.readf64();
            ifj.write(x);
        }
    }

}

pub fn main() void {

}