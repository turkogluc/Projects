<?php session_start(); ?>
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="css/bootstrap.min.css" rel="stylesheet">
<!-- Latest compiled and minified CSS -->
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css">

<!-- Optional theme -->
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap-theme.min.css">

<!-- Latest compiled and minified JavaScript -->
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/js/bootstrap.min.js"></script>	
  </head>
<?php
echo "<pre>"; print_r($_GET); echo "</pre>" ;
//echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>" ;

if(isset($_GET['op'])){


switch($_GET['op']){
	case 'veriGuncelle': 
		//KULLANIM: axajda: excel.php?op=veriGuncelle&satirNo=5&sutunNo=7&yeniVeri=4555 şeklinde çağırılır
		$_SESSION['veri'][$_SESSION['simdikiYaprak']][$_GET['satirNo']][$_GET['sutunNo']] = $_GET['yeniVeri'];
		break;
	
	case 'satirSil':
		$satirNo = $_GET['satirNo']-1;
		$yaprak = $_SESSION['simdikiYaprak'];
		
		unset($_SESSION['veri'][$yaprak][$satirNo]);
		$_SESSION['veri'][$yaprak] = array_values($_SESSION['veri'][$yaprak]);
		
		#echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>" ; 
		header("Refresh: 0; url=save.php");
		break;
	case 'sutunSil':
		$sutunno = $_GET['sutunno']-1;
		$yaprak = $_SESSION['simdikiYaprak'];
		
		foreach($_SESSION['veri'][$yaprak] as $i => $satir){
			foreach($_SESSION['veri'][$yaprak][$i] as $j => $sutun)
				
				unset($_SESSION['veri'][$yaprak][$i][$sutunno]) ;
				$_SESSION['veri'][$yaprak][$i]=array_values($_SESSION['veri'][$yaprak][$i]);
		}
		
		#echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>" ; 
		header("Refresh: 0; url=save.php");
		break;	
	
	case 'satirEkle':
		// sessionda 'veri' elemanınıdaki simdikiYaprak'a bir satır ekler
		$yaprakno = 'yaprak'.($_SESSION['simdikiYaprak']+1);
		$_SESSION['veri'][$yaprakno][] = array();
		$satirsayi = count($_SESSION['veri'][$yaprakno]);
		$sutunsayisi = count($_SESSION['veri'][$yaprakno][0]);
		for($i=0;$i<$sutunsayisi;$i++){
			$_SESSION['veri'][$yaprakno][$satirsayi-1][$i] = ' ';
		}

		#echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>" ; 
		header("Refresh: 0; url=save.php");
		break;
	case 'sutunEkle':
		// // sessionda 'veri' elemanınıdaki simdikiYaprak'taki her satırın sonuna bir boş eleman ekleyerek bir sutun eklemiş olur
		$yaprakno = 'yaprak'.($_SESSION['simdikiYaprak']+1);
		
		foreach($_SESSION['veri'][$yaprakno] as $i => $satir)
			$_SESSION['veri'][$yaprakno][$i][] = ' ';
			
		#echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>" ;
		header("Refresh: 0; url=save.php");
			break;
	case 'yaprakGoster':
		$_SESSION['simdikiYaprak'] = $_GET['yaprakAdi'];
		yaprakGoster($_SESSION['simdikiYaprak']);
		break;
	case 'yeniYaprak':
		$filename = $_SESSION['simdikidosya'];
		$yapraksayisi = count($_SESSION['veri']);
		$yapraksayisi = $yapraksayisi+1;
		$yeniyaprak = 'yaprak'.$yapraksayisi;
		//$_SESSION['veri'][$yeniyaprak] = array();
		$_SESSION['veri'][$yeniyaprak][] = array();
		$_SESSION['veri'][$yeniyaprak][0][] = array();
		$_SESSION['veri'][$yeniyaprak][0][0] = ' ';
		#echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>";
		header("Refresh: 0; url=save.php");
		break;	
	case 'yaprakSil':
		$yaprakAdi = $_GET['yaprakAdi'];
		unset($_SESSION['veri'][$yaprakAdi]);
		$_SESSION['veri'] = array_values($_SESSION['veri']);
		#echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>";
		$yapraksayisi = count($_SESSION['veri']);
		echo $yapraksayisi;
		for($i=0;$i<$yapraksayisi;$i++){
			$ya = 'yaprak'.($i+1) ;
			echo $ya."<br>";
			$_SESSION['veri'][$ya] = $_SESSION['veri'][$i];
			unset($_SESSION['veri'][$i]);
		}
		
		#echo "<pre>"; print_r($_SESSION['veri']); echo "</pre>";
		header("Refresh: 0; url=save.php");
		break;	
	case 'deleteFile':
		$fileName = "dosyalar/".$_GET['fileName'];
		unlink($fileName);
		header("Refresh: 0; url=index.php");
		break;
	case 'createNewFile':
		$filename = "dosyalar/".$_GET['fileName'];
		$f = fopen($filename,"w");
		if(!$f) {
			echo "dosya olusturulamadı !" ;
		}
		$liste = array ('yaprak1,1,1','0');
		foreach ($liste as $satir) {
			fputcsv($f, split(',', $satir));
		}
		fclose($f);	

		header("Refresh: 0; url=index.php");
		
		break;
		
	case 'showFile':
		$filename = "dosyalar/".$_GET['showFileName'];
		
		foreach($_SESSION['veri'] as $key => $val){ // eski tablo session bilgilerinin ezilmesi için
		unset($_SESSION['veri'][$key]);
		}
		$showFileName = $_GET['showFileName'];
		dosyaYukle($showFileName);
		yaprakGoster('yaprak1');
		break;

	default: 
			
	}

}else{  // index'e herhangi bir id gelmedi ise (ilk çalıştırmada)
		
		foreach($_SESSION['veri'] as $key => $val){ // eski tablo session bilgilerinin ezilmesi için
		unset($_SESSION['veri'][$key]);
		}
		
		echo "<form action='' method='GET'>Yeni Dosya Adı:<input type='hidden' name='op' value='createNewFile'><input name='fileName' type='text'> <input type='submit' value='Olustur'></form> <br><br>" ;

		$dir = 'dosyalar' ;
		if (is_dir($dir)) {
			if ($dh = opendir($dir)) {
				while (($file = readdir($dh)) !== false) {
					if($file !== '.' && $file !== '..')
						echo "<a href='?op=showFile&showFileName=$file'>$file</a> | <a href='?op=deleteFile&fileName=$file'> Sil </a> <br> ";
				}
			closedir($dh);
			}
		}		
}

// end of switch

function yaprakGoster($yaprakAdi){
	
	$_SESSION['simdikiYaprak'] = $yaprakAdi;

	echo "<table class='table table-striped'> <tr width='400'> <td><a href='index.php'>Çıkış</a></td> <td>".$_SESSION['simdikidosya'].
	     "<br><a href='save.php'>sakla</a></td>  ";
	$cnt = 0;
	foreach($_SESSION['veri'][$yaprakAdi] as $satirNo => $satir){
		foreach($satir as $sutunNo => $veri) $cnt++;
		break;
	}
	for($i=1;$i<=$cnt;$i++){ 
	echo "<td>$i <br><a href='index.php?op=sutunSil&sutunno=$i'>Sil</a></td>";
	}
	echo "<td><a href='index.php?op=sutunEkle&yaprakNo=$yaprakAdi'>Ekle</td></tr>";
	$i=1;
	foreach($_SESSION['veri'][$yaprakAdi] as $satirNo => $satir){
		
		echo "<tr width='400'>";
		echo "<td><a href='index.php?op=satirSil&satirNo=$i'>Sil</td>";
		echo "<td>$i .</td>";
		$i++;
		foreach($satir as $sutunNo => $veri)
			echo "<td onclick='sihir(this)'>{$veri}</td>";
		echo "</tr>";
	}
	echo "<tr width='400'><td><a href='index.php?op=satirEkle'>Ekle</a></td></tr>";
	echo "<tr><td></td><td></td><td>";
	
	$i = 0;
	while ($f = current($_SESSION['veri'])) {
		$i++;
		$ya = key($_SESSION['veri']);
		if($yaprakAdi != $ya)
			echo "<a href=?op=yaprakGoster&yaprakAdi={$ya}> {$ya} </a> - <a href='index.php?op=yaprakSil&yaprakAdi=$ya'>Sil</a> | ";
		else
			echo "{$ya} </a> - <a href='index.php?op=yaprakSil&yaprakAdi=$ya'>Sil</a> | ";
		
		next($_SESSION['veri']);
	}
	echo "</td><td><a href='index.php?op=yeniYaprak'>Ekle</a></td></tr></table>";
}
function dosyaYukle($file){
	$f = fopen('dosyalar/'.$file,'r');
	while(1){
		list($yaprakAdi, $satirNo, $sutunNo) = fgetcsv($f, ',');
		if(! $yaprakAdi)
			break;
		for($satir = 0; $satir < $satirNo; $satir++)
			$_SESSION['veri'][$yaprakAdi][$satir] = fgetcsv($f);
	}
	$_SESSION['simdikiYaprak'] = 0;
	$_SESSION['simdikidosya'] = $file;	
}
?>

<script>
function sihir(td){
	var yazi = td.innerHTML;
	td.innerHTML = "<input style='border:1px solid blue;' id='f1' name=deger type=text value="+ yazi +" onblur='sakla(this);'>";
	
	var x = document.getElementById("f1");
	x.focus();
	//x.addEventListener("blur", function() { alert('onblur'); sihir(this); }, true);
	//x.addEventListener("focus", function() { alert('onfocus'); sihir(this); }, true);
//	td.onclick = null;
}
function sakla(inputObj){
	//alert('sakla');
	// inputObj.value AJAX ile saklanır
	inputObj.parentNode.onclick = function() { sihir(this); };
	inputObj.parentNode.innerHTML = inputObj.value;
	var selectedCellIndex = inputObje.parentNode.cellIndex;
	var selectedRowIndex = inputObje.parentNode.parentNode.rowIndex;
	var xmlhttp = window.XMLHttpRequest ? new XMLHttpRequest(): new ActiveXObject("Microsoft.XMLHTTP");
	
	xmlhttp.open("GET","save.php?x="+selectedCellIndex+"&y="+selectedRowIndex,true);
	xmlhttp.send();
</script>
