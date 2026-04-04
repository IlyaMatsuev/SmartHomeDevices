# Workflow

This describes how agent roles chain together when implementing a feature or adding a new device.

## Phases

### 1. Architecture (role: `architect`)

Start here for any change that introduces a new device, modifies the MQTT protocol, or changes shared libraries.

**Input**: Feature request or new device idea.
**Output**: A structured plan covering device spec, data model, protocol impact, and file plan.

Skip this phase for small, self-contained changes (bug fixes, tweaking thresholds, updating a single device's logic) where the scope is already clear.

### 2. Implementation (role: `firmware-engineer`)

Implement the plan produced by the architect. If there was no architecture phase, work directly from the feature request.

**Input**: Architect's plan or a well-scoped task.
**Output**: Working code that compiles with `pio run`.

Steps:
1. If the plan modifies shared libraries (`devices/common/`), implement those changes first.
2. Create or modify device-specific files.
3. Verify compilation succeeds for all affected devices.

### 3. Review (role: `reviewer`)

Review the implementation before committing.

**Input**: The diff or set of changed files.
**Output**: Review verdict (approve / request changes) with itemized findings.

If the reviewer requests changes, return to phase 2 to address them, then re-review.

## Phase Flow

```
Feature Request
      |
      v
 [Architect]  -- skip for trivial changes -->  [Firmware Engineer]
      |                                               |
      v                                               v
 [Firmware Engineer]                            [Reviewer]
      |                                            |    |
      v                                     approve |    | request changes
 [Reviewer]                                        v    v
   |    |                                        Done   (back to Firmware Engineer)
   v    v
 Done   (back to Firmware Engineer)
```

## When to Use Which Role

| Scenario | Phases |
|---|---|
| New device from scratch | Architect -> Firmware Engineer -> Reviewer |
| New feature on existing device (e.g., add a sensor) | Architect -> Firmware Engineer -> Reviewer |
| Shared framework change | Architect -> Firmware Engineer -> Reviewer |
| Bug fix or threshold tweak | Firmware Engineer -> Reviewer |
| Config-only change (platformio.ini, pin reassignment) | Firmware Engineer -> Reviewer |
