# OpenAPI

 1. [Introduction](#introduction)
 2. [Generated documentation](#generated-documentation)
    1. [AsciiDoc](#asciidoc)
    2. [Markdown](#markdown)
    3. [HTML](#html)
    4. [ReDoc](#redoc)
    5. [Swagger](#swagger)
 3. [Non-API documentation](#non-api-documentation)
 4. [Other uses](#other-uses)
    1. [Generated clients](#generated-clients)
    2. [Websockets](#websockets)
    3. [Other stuff](#other-stuff)

This is an RFC for documenting FRM's API endpoints, a topic which has come up in both [Discord](https://discord.com/channels/1142919853053841488/1142921725269524643/1296184844107382784) and [other Github issues](https://github.com/porisius/FicsitRemoteMonitoring/issues/105) over the last few days.

## Introduction

Documentation is currently deployed to [docs.ficsit.app](https://docs.ficsit.app/ficsitremotemonitoring/latest/index.html) via the [AsciiDoc files in the main FRM repo](https://github.com/porisius/FicsitRemoteMonitoring/tree/main/docs) and the associated [Antora](https://antora.org/) playbook. The SML docs site uses Antora to combine documentation from multiple source repositories into a single website. There are a couple of potential pain points to this. Since the docs are (as far as I can tell) written manually, it's annoying to update common fields like rotation and easy for example output to fall out of step with the endpoint's description.

One tool I'm aware of which may assist with this is [OpenAPI](https://www.openapis.org/), which allows APIs to be [defined in YAML files](https://swagger.io/docs/specification/v3_0/about/) that can then be converted into documentation and other things. To that end I have written up a very small portion of FRM's 1.0.10 API using OpenAPI. It can be found at [api.yaml](api.yaml). Please note that this was desgined to be a quick example so that I didn't waste time in case a tool like this is not a good fit for FRM, and so there may be typos or inaccuracies in the definition.

## Generated documentation

OpenAPI provides a command-line tool, [OpenAPI-generator](https://github.com/OpenAPITools/openapi-generator), which reads in a YAML definition and spits out documentation. For our purposes there are three formats of interest.

- AsciiDoc
- Markdown
- HTML

Additionally, since OpenAPI is a public specification there are a few third-party tools which support it. There are two that I've looked at.

- ReDoc
- SwaggerUI

There are a lot of others I haven't tried. [Here's a list](https://openapi.tools/#documentation). Anythign with a tick or an "under construction man" symbol in the "3.1" column is likely still maintained, given that the 3.1 spec is relatively new.

### AsciiDoc

Sample output can be seen at [output-samples/default-generator.adoc](output-samples/default-generator.adoc).

An advantage to AsciiDoc is that FRM can continue to use SML's site to host its documentation. Although this example puts the documentation into a single page it is also possible to create a separate page per endpoint, and so the current structure could be mostly preserved.

The main downside is that OpenAI's AsciiDoc output is adequate at best. The formatting is a little verbose and clunky with the schemas under a separate heading rather than being inline with each endpoint. It also shows a lot of headings and columns which probably aren't of interest to users of FRM. Finally, there are several known issues with the openapi-generator for this format which [have been documented for years](https://github.com/OpenAPITools/openapi-generator/issues/11396). These include that it does not correctly generate examples and that it omits field constraints (e.g. minimum and maximum values for numbers). The [available configuration options](https://github.com/OpenAPITools/openapi-generator/blob/master/docs/generators/asciidoc.md#config-options) don't really help, and while it is possible to [modify the templates that openapi-generator uses](https://openapi-generator.tech/docs/templating/#retrieving-templates) solving some of these issues may not be trivial.

Generation of examples will be a recurring theme. While the OpenAPI spec does make is possible to provide a full JSON blob as an example, one advantage to defining the API using the spec is that it *should* be able to generate a full example output from individual example values attached to each entry in the schema, making it harder for the example to differ from the rest of the documentation.

At least one third-party tool exists for generating AsciiDoc: [luftfartsverket/openapi-to-asciidoc](https://github.com/luftfartsverket/openapi-to-asciidoc). In some ways its output is cleaner, though it also appears a little fragile. In testing I found that it started to throw errors as the sample document grew more complex. And so the example I have of its output - found at [output-samples/openapi-to-asciidoc.adoc](output-samples/openapi-to-asciidoc.adoc) - uses a simpler version of the FRM API.

### Markdown

Sample output can be seen at [output-samples/default-generator-md/README.md](output-samples/default-generator-md/README.md).

Markdown can be hosted on [Github wikis](https://docs.github.com/en/communities/documenting-your-project-with-wikis/about-wikis), which come attached to each Github repository.

While this output is definitely cleaner than the the generator's attempt at AsciiDoc, it does suffer from many of the same problems. The split endpoint/schema format is awkward, and no examples or constraints are included in the generated documentation. Once again the [available configuration options](https://github.com/OpenAPITools/openapi-generator/blob/master/docs/generators/markdown.md) don't help and it is also possible to tweak the templates.

### HTML

The generator offers three HTML output formats: "html", "html2", and "dynamic-html". Sample output can be seen at [output-samples/default-generator-html](output-samples/default-generator-html). Github won't render these inline, so download one and open it in your browser if you wish to view it. The "dynamic" generator, which spits out an ExpressJS app, does not seem well-suited to FRM's use case and so was omitted.

HTML can be hosted in a multitude of places. One notable option is [Github Pages](https://pages.github.com/), which comes attached to every Github repository.

This format is included mostly for completeness. While it is the cleanest option described thus far it does suffer from some of the same issues as described above. If there is a desire for HTML-based documentation then ReDoc or Swagger may be better choices.

### ReDoc

Sample output can be seen at [output-samples/redoc-static.html](output-samples/redoc-static.html).

[ReDoc](https://github.com/Redocly/redoc) is the open source (MIT licensed) project that is used to run [Redocly](https://redocly.com/), a commercial API documentation hosting site. In addition to the basic FRM API stub other publicly-available of its output include [Docker](https://docs.docker.com/reference/api/engine/version/v1.25/) and [Discourse](https://docs.discourse.org/). It generates a single HTML file as output which can be deployed to Github Pages, [such as they've done with one of their examples](https://redocly.github.io/redoc/#tag/Operations/operation/getMuseumHours).

The resulting page is easy to navigate, includes examples in the right-hand pane, and correctly shows constraints (see location->rotation as an example). While the sample output was made with the default options there are [a few more which might be of use](https://redocly.com/docs/redoc/config), such as expanding responses by default, grouping endpoints by tag.

### Swagger

Sample output can be seen at [output-samples/swagger.zip](output-samples/swagger.zip). To view, download it, extract it, and open the included index.html.

Swagger is the project which eventually spawned OpenAPI. Much like ReDocly, it consists of a commercial product backed by open-source tools. The [swagger-ui](https://github.com/swagger-api/swagger-ui) tool is used for its web interface, and the ["pet store" example API](https://petstore.swagger.io/) provides another sample of what its output looks like. 

While it can be used to make static HTML, as done here, the process is notably more clumsy than with Redoc and involves either writing a short Javascript app or manually extracting and modifying the swagger-ui's "dist" folder.

The output is better than that produced by openapi-generator, though perhaps not as nice as ReDoc in many ways. It does generate examples though does not show schemas inline.

## Non-API documentation

FRM has a few pieces of documentation which are *not* related to the API endpoints.
- Configuration
- Chat commands
- Websocket and serial usage
- Icon system
- Webhooks
- Contribution information (compiling, etc) (currently being added to README)

The OpenAPI spec includes a "description" field which can contain an arbitrary amount of JSON, including headers. Most documentation generators will show this. An example is in the [Rebilly API docs]([https://developer.zuora.com/v1-api-reference/introduction/](https://www.rebilly.com/catalog/all)) (made with ReDoc) which have a few seconds before the first API, "Allowlists". That would probably be ideal with information on things like WS and and Serial, but documentation designed for users who are just using a premade interface and won't be dealing with the API directly might be better off elsewhere.

What "elsewhere" is depends on how the API docs are presented, but options could include SML AsciiDoc, Github Wiki, another SPA hosted on Github Pages, etc.


## Other uses

Everything described here is entirely optional. It's feasible to write up a YAML file and use it for documentation, then leave the YAML file in the repo and let people generate whatever else they want from it.

### Generated clients

In addition to documentation, OpenAPI can also be used to generate code. The main use of this is for creating client libraries which provide methods for API endpoints, deserialization, and typings where appropriate for the language and framework. The list of [client types supported by openapi-generator](https://github.com/OpenAPITools/openapi-generator/tree/master/docs/generators#client-generators) includes Go, JS, Python, TypeScript, and others. There are also a few variants for things like different JS frameworks.

A few examples are included:
- [output-samples/go]
- [output-samples/python]
- [output-samples/typescript]

If nothing else, the typings might save @derpierre65 some work, as he is currently maintaining derpierre65/FicsitRemoteMonitoring-types. I imagine it would also be helpful for the default web interface as well as projects written in other languages, such as @featheredtoast's work in Go.

### Websockets

Support for async in OpenAPI appears limited to some basic provisions for webhooks. There is a related project, [AsyncAPI](https://github.com/asyncapi), which aims to bridge that gap and can operate on OpenAPI configs. It provides its own generator with [support for a limited set of outputs](https://github.com/asyncapi).

There is also a separate tool which can [generate models for a larger set of languages](https://github.com/asyncapi/modelina#features) though it only reads AsyncAPI definitions (not OpenAPI definitions) as inputs. However, AsyncAPI and OpenAPI are now close enough that it would be feasible to convert one to the other if necessary.

### Other stuff

- OpenAPI can generate server code in addition to client code. This is probably of limited use to FRM. While it does support C++, it only does so for libraries which we aren't using and which would might make WS/serial support more difficult. The aforementioned AsyncAPI tool does have [some support for generating C++ models](https://github.com/asyncapi/modelina/blob/master/docs/usage.md#generate-cplusplus-models), though it may not be useful either.
- There are options for [tests that determine whether API returns the expected results](https://openapi.tools/#testing), though in FRM's case this would likely require testing using a premade save file that is in a known state.
- There are [a few different tools](https://openapi.tools/#mock) which generate mock servers from an API specification that can then be used in testing. However, given that FRM's API is almost entirely "retrieve" operations using those might not be any better than a more basic tool like [json-server](https://github.com/typicode/json-server) or even static JSON files.
- Most of the tools mentioned above are just CLIs that take a single command to generate output, and most are available as Docker containers for easy integration with Github Actions.
- See the [notes.md](notes.md) in this directory for very rough guides on using some of them.
