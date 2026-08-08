# Libs

This directory holds third-party support libraries used by the AppWarrior
framework. Two subdirectories that were present in the original 2003 source
drop have been removed for licensing reasons (see repo history):

- **QuickTime/** — Apple's proprietary QuickTime SDK (headers under
  `CIncludes/`, `ComponentIncludes/`, `RIncludes/`, plus the compiled
  `QD3D.lib` and `qtmlClient.lib` import libraries). This is Apple
  copyrighted/licensed SDK material, not code covered by this project's
  GPL, and isn't appropriate to redistribute in this public repo.
  A developer who wants to build the (optional) QuickTime-based media
  integration will need to obtain Apple's QuickTime SDK separately.
  The current Windows build plan already treats QuickTime support as
  optional/stubbed, so this omission is not blocking.

- **MoreFiles/** — Apple DTS "MoreFiles" sample code
  (© 1992-2001 Apple Computer, Inc.), distributed under Apple's own
  sample-code terms (redistribution allowed only with attribution and
  only if unmodified, or with changes clearly marked as such) rather
  than this project's GPL. Removed for the same reason as QuickTime.
  Note: some files under `Libs/QTDataHandler/` `#include` MoreFiles
  headers — a build targeting that code path will need a replacement
  or a separately-sourced copy of MoreFiles.

Two other subdirectories with generic third-party-sounding names were
checked and left in place, since they either contain no source (only
binary `.rsrc` resource files with no clear licensing header) or are
empty in this checkout:

- **Internet Config/** — contains only compiled `.rsrc` resource files
  (icons, sample-app resources) from Apple/ICProgKit-era sample kits;
  no `.c`/`.h` source with a licensing header was found to evaluate.
  Left in place; flagged for a human to double check if this ever
  needs closer review.
- **MoreFiles PPC/** and **Minimal IC APIs/** — empty in this checkout
  (no tracked files); nothing to remove.
