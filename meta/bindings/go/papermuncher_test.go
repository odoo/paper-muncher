package papermuncher

import (
	"bytes"
	"context"
	"reflect"
	"strings"
	"testing"
)

func TestBuildPrintArgs(t *testing.T) {
	config := PrintConfig{
		scale:        "2x",
		density:      "96dpi",
		width:        "800px",
		height:       "600px",
		paper:        "A3",
		orientation:  "landscape",
		outputFormat: "application/postscript",
		unsecure:     true,
		verbose:      true,
	}

	expected := []string{
		"print", "-", "-o", "-",
		"--scale", "2x",
		"--density", "96dpi",
		"--width", "800px",
		"--height", "600px",
		"--paper", "A3",
		"--orientation", "landscape",
		"--format", "application/postscript",
		"--unsecure",
		"--verbose",
	}

	args := buildPrintArgs(config)
	if !reflect.DeepEqual(args, expected) {
		t.Errorf("Expected:\n%v\nGot:\n%v", expected, args)
	}
}

func TestBuildRenderArgs(t *testing.T) {
	config := RenderConfig{
		scale:        "192dpi",
		density:      "144dpi",
		width:        "1024px",
		height:       "768px",
		outputFormat: "image/png",
		wireframe:    true,
		unsecure:     true,
		verbose:      true,
	}

	expected := []string{
		"render", "-", "-o", "-",
		"--scale", "192dpi",
		"--density", "144dpi",
		"--width", "1024px",
		"--height", "768px",
		"--format", "image/png",
		"--wireframe",
		"--unsecure",
		"--verbose",
	}

	args := buildRenderArgs(config)
	if !reflect.DeepEqual(args, expected) {
		t.Errorf("Expected:\n%v\nGot:\n%v", expected, args)
	}
}

func TestPrintIntegration(t *testing.T) {
	var output bytes.Buffer
	input := strings.NewReader("<html><body><h1>Hello</h1></body></html>")

	err := Print(context.Background(), input, &output,
		WithPrintFormat("application/pdf"),
	)
	if err != nil {
		t.Fatalf("Print failed: %v", err)
	}

	if output.Len() == 0 {
		t.Error("expected non-empty PDF output")
	}
}

func TestRenderIntegration(t *testing.T) {
	var output bytes.Buffer
	input := strings.NewReader("<html><body><h1>Hello</h1></body></html>")

	err := Render(context.Background(), input, &output,
		WithRenderFormat("image/png"),
	)
	if err != nil {
		t.Fatalf("Render failed: %v", err)
	}

	if output.Len() == 0 {
		t.Error("expected non-empty image output")
	}
}
