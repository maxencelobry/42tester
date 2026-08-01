NAME    := 42tester
VERSION := $(shell git describe --tags --always --dirty 2>/dev/null || echo dev)
LDFLAGS := -s -w -X main.version=$(VERSION)

all: $(NAME)

$(NAME):
	go build -ldflags "$(LDFLAGS)" -o $(NAME) ./cmd/42tester

# The three platforms 42 students actually use, plus Windows for the ones on
# a personal machine. Everything is embedded, so each binary stands alone.
release:
	mkdir -p dist
	GOOS=darwin  GOARCH=arm64 go build -ldflags "$(LDFLAGS)" -o dist/$(NAME)-macos-arm64 ./cmd/42tester
	GOOS=darwin  GOARCH=amd64 go build -ldflags "$(LDFLAGS)" -o dist/$(NAME)-macos-amd64 ./cmd/42tester
	GOOS=linux   GOARCH=amd64 go build -ldflags "$(LDFLAGS)" -o dist/$(NAME)-linux-amd64 ./cmd/42tester
	GOOS=linux   GOARCH=arm64 go build -ldflags "$(LDFLAGS)" -o dist/$(NAME)-linux-arm64 ./cmd/42tester
	GOOS=windows GOARCH=amd64 go build -ldflags "$(LDFLAGS)" -o dist/$(NAME)-windows-amd64.exe ./cmd/42tester

test:
	go test ./...

vet:
	go vet ./...

clean:
	rm -f $(NAME) $(NAME).exe
	rm -rf dist

.PHONY: all release test vet clean $(NAME)
