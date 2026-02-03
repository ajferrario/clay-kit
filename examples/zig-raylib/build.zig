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

    // Create claykit module from our hand-written bindings
    const claykit_module = b.createModule(.{
        .root_source_file = b.path("../../clay_kit.zig"),
        .target = target,
        .optimize = optimize,
    });
    // claykit depends on zclay
    claykit_module.addImport("zclay", zclay_dep.module("zclay"));

    // Create the root module
    const root_module = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // Add imports to the root module
    root_module.addImport("zclay", zclay_dep.module("zclay"));
    root_module.addImport("raylib", raylib_dep.module("raylib"));
    root_module.addImport("claykit", claykit_module);

    // Create executable with the module
    const exe = b.addExecutable(.{
        .name = "claykit-demo",
        .root_module = root_module,
    });

    // Compile clay_kit.c (C implementation)
    exe.addCSourceFile(.{
        .file = b.path("../../clay_kit.c"),
        .flags = &.{"-std=c99"},
    });
    exe.addIncludePath(b.path("../../"));
    exe.addIncludePath(b.path("../../vendor"));

    // Link raylib
    const raylib_artifact = raylib_dep.artifact("raylib");
    exe.linkLibrary(raylib_artifact);

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
