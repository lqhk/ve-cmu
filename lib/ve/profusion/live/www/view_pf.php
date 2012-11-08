<?
require_once("pfcmu.inc");
$PF=$_GET['pf'];
?>
<html>
<header>
<script type="text/javascript" src="reload.js"></script>
<style type-"text/css">
<!--
img {
  border: none;
}
a {
  border: none;
  text-decoration: none;
}
-->
</style>
</header>
<body>
<div name="view_pf">
<h1>ProFusion-CMU #<?=$PF?></h1>
<?= live("ve$PF", sprintf("/live/ve%02d/thumb.jpg", $PF), 50, 640, 480, "usemap=\"#linkmap\""); ?>

<map name="linkmap">
<?
for($y=0 ; $y<5 ; $y++) {
  for($x=0 ; $x<5 ; $x++) {
    if($y * 5 + $x == 24) break;
    printf("<area shape=\"rect\" coords=\"%d,%d,%d,%d\" href=\"view_cam.php?pf=$PF&cam=%d\">\n", $x * 128, $y * 96, ($x + 1)*128-1, ($y + 1)*96-1, $y*5 + $x + 1);
  }
}
?>
</map>
</div>
<div name="navi">
<a href="view_pf.php?pf=<?= ($PF + 38)%40 + 1 ?>">[Prev]</a>
<a href="view_all.php">[Up]</a>
<a href="view_pf.php?pf=<?= ($PF)%40 + 1 ?>">[Next]</a>
</div>
<hr>
<div name="status">
<h4>Status</h4>
<ul>
<li>Description: <span id="desc">N/A</span></li>
<li>Frame: <span id="curr">N/A</span> / <span id="total">N/A</span> (framedrop=<span id="framedrop">N/A</span>)</li>
<li>Timestamp: <span id="timestamp">N/A</span></li>
<li>Shutter: <span id="shutter">N/A</span></li>
<li>Gain: <span id="gain">N/A</span></li>
</ul>
</div>
<script type="text/javascript">
refreshStatus('/live/ve<?=$PF?>/info.json', 0);
</script>
</body>
</html>

