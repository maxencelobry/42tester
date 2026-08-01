// Package harness embeds the C test suite in the binary so students only
// ever need to download one file.
package harness

import (
	"embed"
	"io/fs"
	"os"
	"path/filepath"
)

//go:embed all:c
var files embed.FS

// FS returns the harness tree rooted at the C sources.
func FS() fs.FS {
	sub, err := fs.Sub(files, "c")
	if err != nil {
		panic(err) // impossible: the directory is embedded at build time
	}
	return sub
}

// Extract writes the harness to dir so the compiler can reach it.
func Extract(dir string) error {
	root := FS()
	return fs.WalkDir(root, ".", func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		target := filepath.Join(dir, filepath.FromSlash(path))
		if d.IsDir() {
			return os.MkdirAll(target, 0o755)
		}
		data, err := fs.ReadFile(root, path)
		if err != nil {
			return err
		}
		if err := os.MkdirAll(filepath.Dir(target), 0o755); err != nil {
			return err
		}
		return os.WriteFile(target, data, 0o644)
	})
}
