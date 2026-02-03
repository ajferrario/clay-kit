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

    // Get clay dependency (same version as zclay uses)
    const clay_dep = b.dependency("clay", .{});

    // Build clay library using addLibrary
    const clay_lib = b.addLibrary(.{
        .name = "clay",
        .linkage = .static,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });
    clay_lib.addCSourceFile(.{
        .file = b.addWriteFiles().add("clay.c",
            \\#define CLAY_IMPLEMENTATION
            \\#include "clay.h"
        ),
        .flags = &.{"-std=c99"},
    });
    clay_lib.addIncludePath(clay_dep.path(""));

    // Link clay library to executable
    exe.linkLibrary(clay_lib);

    // Compile clay_kit.c
    exe.addCSourceFile(.{
        .file = b.path("../../clay_kit.c"),
        .flags = &.{"-std=c99"},
    });
    exe.addIncludePath(b.path("../../"));
    exe.addIncludePath(clay_dep.path(""));

    // Link raylib
    const raylib_artifact = raylib_dep.artifact("raylib");
    exe.linkLibrary(raylib_artifact);

    // Install
    b.installArtifact(exe);

    // Install resources
    b.installDirectory(.{
        .source_dir = b.path("src/resources"),
        .install_dir = .{ .custom = "bin" },
        .install_subdir = "resources",
    });

    // Run step
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the demo application");
    run_step.dependOn(&run_cmd.step);
}
