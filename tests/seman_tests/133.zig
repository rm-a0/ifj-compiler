const ifj = @import("ifj24.zig");

pub fn main() void {
    var n = ifj.readi32();
    var faktorial: i32 = 1;

    while (n > 0) {
        faktorial = faktorial * n;
        n = n - 1;
    }

    ifj.write("Faktorial je: ");
    ifj.write(faktorial);
    ifj.write("\n");
}
