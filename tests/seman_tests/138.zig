const ifj = @import("ifj24.zig");

pub fn main() void {
    var x = ifj.readf64();
    var cycle_counter = 0;

    while (x < 15.013) {
        x = x + 0.001;
        cycle_counter = cycle_counter + 1;
    }
    ifj.write(cycle_counter);
}