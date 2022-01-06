def _check_header(header):
    header = header.upper().replace(".", "_")
    return header

def _rule_config_impl(ctx):
    out = ctx.actions.declare_file(ctx.attr.output)
    ctx.toolchains
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {
            "{NAME}": _check_header("stdbool.h"),
        },
    )
    return [DefaultInfo(files = depset([out]))]

rule_config = rule(
    implementation = _rule_config_impl,
    attrs = {
        "output": attr.string(mandatory = True),
        "template": attr.label(
            allow_single_file = [".c.tpl"],
            mandatory = True,
        ),
    },
)
