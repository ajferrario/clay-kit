const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Get dependencies
    const zclay_dep = b.dependency("zclay", .{
        .target = target,
        .optimize = optimize,
    });
    const raylib_dep = b.dependency("raylib_zig", .{
        .target = target,
        .optimize = optimize,
    });

    // Create executable
    const exe = b.addExecutable(.{
        .name = "claykit-demo",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // Add clay-zig module
    exe.root_module.addImport("zclay", zclay_dep.module("zclay"));

    // Add raylib module
    const raylib_mod = raylib_dep.module("raylib");
    exe.root_module.addImport("raylib", raylib_mod);

    // Link raylib
    raylib_dep.artifact("raylib").addIncludePath(b.path("../../"));
    exe.linkLibrary(raylib_dep.artifact("raylib"));

    // Add include path for clay_kit.h (relative to this build.zig)
    exe.root_module.addIncludePath(b.path("../../"));
    exe.root_module.addIncludePath(b.path("../../vendor"));

    // Install
    b.installArtifact(exe);

    // Run step
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the demo application");
    run_step.dependOn(&run_cmd.step);
}
