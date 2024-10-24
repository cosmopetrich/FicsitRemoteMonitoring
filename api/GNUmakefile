api_yaml_defs = openapi/frm-openapi.yaml openapi/resources/*.yaml
api_def = frm-openapi.json
client_ts = clients/typescript

# Merge the multiple YAML files into a single JSON file
# It's portable and JSON seem slightly more widely-supported by OpenAPI tooling
# Plus this is a convenient time to set a version number
$(api_def): $(api_yaml_defs)
	@rm -f $@
	$(eval TMPDIR := $(shell mktemp -d))
	$(eval TMPFILE := $(TMPDIR)/frm-openapi.json)
	openapi bundle openapi/frm-openapi.yaml -o $(TMPFILE)
	yq e -i '.info.version = "$(VERSION)"' $(TMPFILE)
	@mv $(TMPFILE) $@

redoc-static.html: $(api_def)
	redocly build-docs --config redocly.yaml --output $@ $<

$(client_ts): $(api_def)
	npx @hey-api/openapi-ts -i $< -o $@ -c @hey-api/client-fetch

clients: $(client_ts)

.PHONY: check-yaml
check-yaml: $(api_yaml_defs)
	yq $^ > /dev/null

.PHONY: lint
lint: $(api_def)
	spectral lint $<