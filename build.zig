const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Get dependencies
    const zclay_dep = b.dependency("zclay", .{
        .target = target,
        .optimize = optimize,
    });
    const clay_dep = b.dependency("clay", .{});

    // Build clay as a static library
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

    // Build clay_kit as a static library
    const claykit_lib = b.addLibrary(.{
        .name = "clay_kit",
        .linkage = .static,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });
    claykit_lib.addCSourceFile(.{
        .file = b.path("clay_kit.c"),
        .flags = &.{"-std=c99"},
    });
    claykit_lib.addIncludePath(b.path(""));
    claykit_lib.addIncludePath(clay_dep.path(""));
    claykit_lib.linkLibrary(clay_lib);

    b.installArtifact(clay_lib);
    b.installArtifact(claykit_lib);

    // Create claykit Zig module
    const claykit_module = b.addModule("claykit", .{
        .root_source_file = b.path("clay_kit.zig"),
        .target = target,
        .optimize = optimize,
    });
    claykit_module.addImport("zclay", zclay_dep.module("zclay"));
}
