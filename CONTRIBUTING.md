# Contributing to trax

Thank you for offering to contribute to this repository. The trax project is "source-available" (not open source). That means contributions and the way we license/accept them require explicit permission or transfer of rights from each contributor. This document explains our process, the rights we need, and practical steps to contribute.

This file is not legal advice. If you have questions about the legal text below or the implications of assigning or licensing your copyright, please consult an attorney or contact the maintainers at horstmann.marc@trendverlag.de.

## Summary (short)
- We accept contributions (patches, PRs, docs, tests).
- Before we can use a contribution in our codebase we need a clear right to do so. You can:
  1. Grant Trend?Verlag a Contributor License (a broad, royalty?free license), OR
  2. Assign copyright to Trend?Verlag (only when necessary), OR
  3. Use a DCO-style sign-off to confirm you have the right to contribute if you and the maintainers agree this is sufficient.
- To contribute, follow the steps below and include the required legal confirmation (signed CLA/assignment or commit sign-off) as described.

## Before you contribute
1. Confirm you are allowed to contribute the work (it is your original work or you are authorised by your employer).
2. Decide which rights option you prefer (license grant, assignment, or DCO sign-off). If you want to negotiate a different license, contact the maintainers before submitting code.
3. If your employer requires, obtain a signed employer authorization or have them sign the CLA.

## Which rights option should I choose?
We prefer contributors to grant a license, because assignments (full copyright transfer) are rarely necessary and can discourage contributors. Below are the options we accept:

A) Contributor License Grant (recommended)
- You grant Trend?Verlag a perpetual, worldwide, royalty?free, transferable, sublicensable license to use, copy, modify, distribute, and otherwise exploit your contribution.
- This keeps you as the copyright holder while giving us the rights we need.

B) Copyright Assignment (rare; only when required)
- You assign all copyright in the contribution to Trend?Verlag. This is a stronger transfer of rights and will be used only when absolutely necessary.
- If you prefer to negotiate terms/compensation, please contact the maintainers before submitting.

C) Developer Certificate of Origin (DCO) sign?off (lightweight)
- You add a `Signed-off-by: Your Name <you@example.com>` line to each commit and affirm that you have the right to submit the contribution under the repository's stated license/terms.
- DCO does not transfer copyright; it asserts provenance. We may accept DCO-only contributions for low-risk changes, but for significant contributions we will request the formal license grant or assignment.

## How to provide the legal confirmation
- Option A (CLA license grant):
  - Sign the text under "Contributor License Agreement (License Grant)" below, then either:
    - Upload the signed PDF to this repository in a private folder (or attach to the PR), or
    - Email the signed PDF to horstmann.marc@trendverlag.de and reference the PR/patch.
- Option B (Assignment):
  - Sign the "Copyright Assignment" text below and follow the same delivery steps as CLA.
- Option C (DCO):
  - Add a `Signed-off-by` line to your commit message:
    - Example: `git commit -s -m "Fix: adjust track curve calculations"`
  - The DCO text below is used to record your affirmation.

If you are unsure which to use, choose the license grant (A) and contact maintainers to discuss alternatives.

## Contributor process (technical)
1. Create a branch from `main` named `contrib/<yourname>-<short-description>` (or follow org workflow).
2. Make small, focused commits with clear messages. We recommend the "Conventional Commits" style, but it's not mandatory.
3. For DCO-style contributions, include `Signed-off-by: Full Name <email>` in the commit message (`git commit -s`).
4. Push your branch and open a Pull Request against `main`. If you do not have push access, send a patch or email per maintainers' instructions.
5. In your PR description include:
   - What the change is and why.
   - Which tests you ran and how.
   - Link to any related issue.
   - Which legal option you are using (CLA signed, assignment signed, or DCO sign-off).
6. Ensure CI/tests pass. We will not merge code that breaks the test suite or introduces known security issues.

## PR checklist (you should complete before requesting review)
- [ ] I have read and followed the CONTRIBUTING guidelines.
- [ ] I have the right to submit this work (it's my original work, or I have employer permission).
- [ ] I have attached/sent a signed CLA or assignment, or I added DCO sign-offs as indicated.
- [ ] I added tests for any new behavior and all tests pass locally.
- [ ] I updated documentation if appropriate (README, docs/).
- [ ] I verified no secrets, credentials, or private data are included.

## Code style, tests, and CI
- Respect the existing code style. We use the project's formatting and linters; run them locally where possible.
- Run tests locally and make sure CI passes before requesting a review.
- Add unit tests for new functionality and update or add integration tests where applicable.

Suggested local commands (adjust to your environment):
- Build: ./scripts/build or cmake / make as per the repo
- Tests: ctest or ./scripts/test
- Format: clang-format or repo-specific formatter

## Security and sensitive data
- Do not include secrets or private credentials in contributions.
- If you discover a security vulnerability, do not open a public issue. Email security reports to horstmann.marc@trendverlag.de.

## Employer / organization contributions
- If you are contributing as part of your employment, you must ensure your employer allows the contribution and sign the CLA or provide employer authorization. We may request proof of authorization.

## Attribution and authorship
- We will retain authorship metadata in commits and attribute contributions in changelogs where appropriate.

## Process for negotiating different terms
- If you need to negotiate license terms (for compensation, different license, or limited license), contact the maintainers BEFORE making a large contribution. Provide a short description of the requested terms and the reasons.

Contact: horstmann.marc@trendverlag.de

## Sample legal texts

Below are samples you can sign. They are provided for convenience — maintainers may adapt the final text. Signing any text should be done by printing, signing, and emailing a PDF, or by using an agreed electronic signature process.

### Contributor License Agreement (License Grant) — sample
I, the undersigned, hereby grant to Trend-Verlag and its affiliates a perpetual, worldwide, non-exclusive, royalty?free, transferable, sublicensable license to use, reproduce, modify, prepare derivative works of, distribute, publicly perform, and publicly display the Contribution (as defined below) and to incorporate the Contribution into software or other works, in source or binary form, for any purpose.

"Contribution" means any original work of authorship, including any modifications or additions to existing works, that is intentionally submitted by me to Trend?Verlag for inclusion in, or documentation of, the repository.

I represent that I am legally entitled to grant this license (I am the author or have the necessary rights from my employer or other right holders) and that the Contribution does not violate any third-party rights.

Signature: ______________________
Name: _________________________
Email: _________________________
Date: __________________________

### Copyright Assignment — sample (use only if requested)
I hereby assign to Trend?Verlag all of my right, title and interest in and to the Contribution, including copyright, and any renewals and extensions thereof, worldwide. I acknowledge that Trend?Verlag will own the Contribution and will have the exclusive right to use, license, and enforce copyright in the Contribution.

Signature: ______________________
Name: _________________________
Email: _________________________
Date: __________________________

### Developer Certificate of Origin (DCO) — sample statement
By making a contribution to this repository, I certify that:
- I wrote the contribution or otherwise have the right to submit it under the repository terms, and
- I license it to Trend?Verlag under the repository's accepted terms.
I agree to add a `Signed-off-by: Name <email>` line to my commits to indicate this.

Example commit sign-off:
Signed-off-by: Jane Doe <jane@example.com>

## Miscellaneous
- We may request additional documentation or a discussion for large or sensitive contributions.
- We reserve the right to refuse any contribution that does not comply with this policy or that we determine, in our discretion, creates legal risk.

Thank you for helping improve trax — we appreciate your time and contributions.

If you'd like, we can:
- Provide a PDF version of the CLA or assignment for signing,
- Add an automated check that a given PR includes a "CLA: yes" label,
- Or draft a short README paragraph linking to this CONTRIBUTING.md.
