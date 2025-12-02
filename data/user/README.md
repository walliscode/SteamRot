# User Data Directory

This directory contains user-specific data that can override default settings.

## Directory Structure

- `preferences/` - User preferences that override default preferences
- `saves/` - Game save files

## How It Works

The engine uses a cascading configuration system:
1. First, default configuration is loaded from `defaults/`
2. Then, if user-specific files exist in `user/`, they override the defaults

This allows users to customize their experience while maintaining clean defaults.
