<?php require("docgen.inc"); ?>

<?php BeginPage('psx', 'Sony PlayStation'); ?>

<?php BeginSection('Introduction'); ?>
PlayStation 1 emulation is currently in a very buggy alpha state and not all hardware and features are emulated.
<p>
If compiling from source, you must pass "--enable-psx" to "configure" to compile with PlayStation 1 emulation enabled.
</p>

<p>
PSF1 playback is supported.
</p>
<?php EndSection(); ?>

<?php PrintSettings(); ?>

<?php EndPage(); ?>

