# Design Document Template

Status: DRAFT
Author: YOUR NAME <you@example.com>
Date: YYYY-MM-DD
Reviewers: @owner1, @owner2

## Title
A short, descriptive title for the design.

## Summary / Executive Summary
1–3 sentences describing what this proposal is and the high-level outcome.

## Goals
A short bullet list of the goals this design intends to achieve.

## Non-Goals
What this design explicitly does not address.

## Background and Context
Why this work is needed. Include links to issues, user stories, telemetry, or customer requests.

## Requirements
- Functional requirements (what the system must do)
- Non-functional requirements (performance, scalability, reliability, security, compatibility)

## Proposed Design
High-level description of the architecture and approach. Include diagrams where helpful (Mermaid, PNG, or SVG). Describe components, their responsibilities, and how they interact.

### Data Flow / Sequence
Describe how data moves through the system, including inputs/outputs and any transformations or validations.

### Interfaces / APIs
Document public interfaces, API endpoints, message formats, configuration knobs, and example requests/responses.

### Data Model and Storage
Describe schemas, data formats, storage systems, and any required migrations.

## Security & Privacy Considerations
List threat model, sensitive data handling, access controls, and any mitigations.

## Testing and Validation
How the feature will be tested (unit, integration, performance tests), and what metrics or observations will demonstrate success.

## Rollout and Migration Plan
How the feature will be rolled out (feature flags, phased rollout), compatibility strategy, and migration steps for existing data or users.

## Monitoring & Operational Concerns
What logs, metrics, and alerts will be added; how to debug and operate the feature in production.

## Alternatives Considered
Briefly list alternatives and why they were rejected.

## Drawbacks and Open Questions
Any known downsides, unresolved questions, or risks.

## Dependencies
Any internal teams, external libraries, or infrastructure required.

## Timeline & Milestones
High-level timeline and key milestones.

## Decision Log
When decisions are made, record what was decided, who approved, and the rationale.

## Appendix
Benchmarks, design sketches, or other reference material.

---

How to use this template
- Create a new file in docs/designs/ named `YYYY-MM-DD-<short-title>-design.md` and use this template as a starting point.
- Open a Pull Request titled `Design: <short title>` so reviewers can comment inline.
- After approval, move the status to `APPROVED` and link the implementation PRs back to the design doc.