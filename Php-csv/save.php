<?php
session_start();
#echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>" ;

$file = 'dosyalar/'.$_SESSION['simdikidosya'];
$f =fopen($file,'w+');
if(!$f) {
			echo "dosya acilamadi !" ;
		}
	
foreach($_SESSION['veri'] as $key => $value ){
	$satirsayisi = count($value);
	
	foreach($value as $satir){
		$sutunsayisi = count($satir);
	}
	
	$stack = array($key,$satirsayisi,$sutunsayisi);
	fputcsv($f,$stack);
	
	
	foreach($value as $satir => $sutun){
		$stack2 = array();
		foreach($sutun as $value2){
			
			array_push($stack2, $value2);
		}
		fputcsv($f,$stack2);
		
		unset($stack2);
	}
	

}
	$go = $_SESSION['simdikidosya'];
	header("Refresh: 10; url=index.php?op=showFile&showFileName=$go");

?>