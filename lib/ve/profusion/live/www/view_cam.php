<?
require_once("pfcmu.inc");
$PF=$_GET['pf'];
$CAM=$_GET['cam'];
?>
<html>
<header>
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
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
<h1>ProFusion-CMU #<?=$PF?>, CAM=<?=$CAM?></h1>
<?= live("ve$PF", sprintf("/live/ve%02d/%02d.jpg", $PF, $CAM), 50, 640, 480); ?>
<div name="navi">
<a href="view_cam.php?pf=<?= $PF ?>&cam=<?= ($CAM + 22)%24 + 1 ?>">[Prev]</a>
<a href="view_pf.php?pf=<?= $PF ?>">[Up]</a>
<a href="view_cam.php?pf=<?= $PF ?>&cam=<?= ($CAM)%24 + 1 ?>">[Next]</a>
</div>
<hr>
<div name="status">
<h4>Status</h4>
<ul>
<li>Description: <span id="desc">N/A</span></li>
<li>Frame: <span id="curr">N/A</span> / <span id="total">N/A</span> (framedrop=<span id="framedrop">N/A</span>)</li>
<li>Timestamp: <span id="timestamp">N/A</span></li>
<li>Shutter: <span id="shutter">N/A</span> (auto=<span id="shutter_auto">N/A</span>)</li>
<li>Gain: <span id="gain">N/A</span> (auto=<span id="gain_auto">N/A</span>)</li>
</ul>
</div>
<script type="text/javascript">
refreshStatus('/live/ve<?=$PF?>/info.json', <?=$CAM?>);
</script>
</body>
</html>

