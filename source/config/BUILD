load(":config.bzl", "rule_config")

rule_config(
    name = "config",
    output = "config.cc",
    template = "config.c.tpl",
)

cc_binary(
    name = "event_config",
    srcs = [":config"],
)