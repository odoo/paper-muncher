package papermuncher

import (
	"bytes"
	"context"
	"fmt"
	"io"
	"os/exec"
)

// PrintConfig holds configuration for the print command
type PrintConfig struct {
	scale        string
	density      string
	width        string
	height       string
	paper        string
	orientation  string
	outputFormat string
	unsecure     bool
	verbose      bool
}

// PrintOption configures Print options
type PrintOption func(*PrintConfig)

// WithPrintScale sets the scale (e.g., "2x")
func WithPrintScale(scale string) PrintOption {
	return func(c *PrintConfig) {
		c.scale = scale
	}
}

// WithPrintDensity sets the density (e.g., "96dpi")
func WithPrintDensity(density string) PrintOption {
	return func(c *PrintConfig) {
		c.density = density
	}
}

// WithPrintWidth sets the width (e.g., "800px")
func WithPrintWidth(width string) PrintOption {
	return func(c *PrintConfig) {
		c.width = width
	}
}

// WithPrintHeight sets the height (e.g., "600px")
func WithPrintHeight(height string) PrintOption {
	return func(c *PrintConfig) {
		c.height = height
	}
}

// WithPaperSize sets the paper size (e.g., "A4")
func WithPaperSize(paper string) PrintOption {
	return func(c *PrintConfig) {
		c.paper = paper
	}
}

// WithOrientation sets the page orientation (e.g., "landscape")
func WithOrientation(orientation string) PrintOption {
	return func(c *PrintConfig) {
		c.orientation = orientation
	}
}

// WithPrintFormat sets the output format (e.g., "application/pdf")
func WithPrintFormat(format string) PrintOption {
	return func(c *PrintConfig) {
		c.outputFormat = format
	}
}

// WithUnsecure enables insecure transports
func WithUnsecure(unsecure bool) PrintOption {
	return func(c *PrintConfig) {
		c.unsecure = unsecure
	}
}

// WithVerbose enables verbose logging
func WithVerbose(verbose bool) PrintOption {
	return func(c *PrintConfig) {
		c.verbose = verbose
	}
}

// Print converts an input document to a printable format
func Print(ctx context.Context, input io.Reader, output io.Writer, opts ...PrintOption) error {
	config := PrintConfig{
		scale:        "1x",
		density:      "1x",
		paper:        "A4",
		orientation:  "portrait",
		outputFormat: "application/pdf",
	}
	for _, opt := range opts {
		opt(&config)
	}

	args := buildPrintArgs(config)
	cmd := exec.CommandContext(ctx, "paper-muncher", args...)
	cmd.Stdin = input
	cmd.Stdout = output

	var stderr bytes.Buffer
	cmd.Stderr = &stderr

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("paper-muncher print failed: %v: %s", err, stderr.String())
	}
	return nil
}

func buildPrintArgs(config PrintConfig) []string {
	args := []string{"print", "-", "-o", "-"}

	if config.scale != "" {
		args = append(args, "--scale", config.scale)
	}
	if config.density != "" {
		args = append(args, "--density", config.density)
	}
	if config.width != "" {
		args = append(args, "--width", config.width)
	}
	if config.height != "" {
		args = append(args, "--height", config.height)
	}
	if config.paper != "" {
		args = append(args, "--paper", config.paper)
	}
	if config.orientation != "" {
		args = append(args, "--orientation", config.orientation)
	}
	if config.outputFormat != "" {
		args = append(args, "--format", config.outputFormat)
	}
	if config.unsecure {
		args = append(args, "--unsecure")
	}
	if config.verbose {
		args = append(args, "--verbose")
	}

	return args
}

// RenderConfig holds configuration for the render command
type RenderConfig struct {
	scale        string
	density      string
	width        string
	height       string
	outputFormat string
	wireframe    bool
	unsecure     bool
	verbose      bool
}

// RenderOption configures Render options
type RenderOption func(*RenderConfig)

// WithRenderScale sets the render scale
func WithRenderScale(scale string) RenderOption {
	return func(c *RenderConfig) {
		c.scale = scale
	}
}

// WithRenderDensity sets the render density
func WithRenderDensity(density string) RenderOption {
	return func(c *RenderConfig) {
		c.density = density
	}
}

// WithRenderWidth sets the render width
func WithRenderWidth(width string) RenderOption {
	return func(c *RenderConfig) {
		c.width = width
	}
}

// WithRenderHeight sets the render height
func WithRenderHeight(height string) RenderOption {
	return func(c *RenderConfig) {
		c.height = height
	}
}

// WithRenderFormat sets the output format
func WithRenderFormat(format string) RenderOption {
	return func(c *RenderConfig) {
		c.outputFormat = format
	}
}

// WithWireframe enables wireframe mode
func WithWireframe(wireframe bool) RenderOption {
	return func(c *RenderConfig) {
		c.wireframe = wireframe
	}
}

// WithRenderUnsecure enables insecure transports for render
func WithRenderUnsecure(unsecure bool) RenderOption {
	return func(c *RenderConfig) {
		c.unsecure = unsecure
	}
}

// WithRenderVerbose enables verbose logging for render
func WithRenderVerbose(verbose bool) RenderOption {
	return func(c *RenderConfig) {
		c.verbose = verbose
	}
}

// Render converts an input document to an image format
func Render(ctx context.Context, input io.Reader, output io.Writer, opts ...RenderOption) error {
	config := RenderConfig{
		scale:        "96dpi",
		density:      "96dpi",
		width:        "800px",
		height:       "600px",
		outputFormat: "image/bmp",
	}
	for _, opt := range opts {
		opt(&config)
	}

	args := buildRenderArgs(config)
	cmd := exec.CommandContext(ctx, "paper-muncher", args...)
	cmd.Stdin = input
	cmd.Stdout = output

	var stderr bytes.Buffer
	cmd.Stderr = &stderr

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("paper-muncher render failed: %v: %s", err, stderr.String())
	}
	return nil
}

func buildRenderArgs(config RenderConfig) []string {
	args := []string{"render", "-", "-o", "-"}

	if config.scale != "" {
		args = append(args, "--scale", config.scale)
	}
	if config.density != "" {
		args = append(args, "--density", config.density)
	}
	if config.width != "" {
		args = append(args, "--width", config.width)
	}
	if config.height != "" {
		args = append(args, "--height", config.height)
	}
	if config.outputFormat != "" {
		args = append(args, "--format", config.outputFormat)
	}
	if config.wireframe {
		args = append(args, "--wireframe")
	}
	if config.unsecure {
		args = append(args, "--unsecure")
	}
	if config.verbose {
		args = append(args, "--verbose")
	}

	return args
}
