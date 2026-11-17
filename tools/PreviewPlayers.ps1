# Static visual QA with the actual firmware masks. Not a hardware/touch test.
Add-Type -AssemblyName System.Drawing
$root=Split-Path $PSScriptRoot
$text=[IO.File]::ReadAllText((Join-Path $root 'SmoothTextData.h'))
$digits=[IO.File]::ReadAllText((Join-Path $root 'SmoothDigitsData.h'))
$icons=[IO.File]::ReadAllText((Join-Path $root 'SmoothIconsData.h'))
$cache=@{}
function Get-Mask($source,$name) {
  if(!$cache.ContainsKey($name)) {
    $m=[regex]::Match($source,([regex]::Escape($name)+'\[\] PROGMEM = \{([^}]+)\}'))
    if(!$m.Success){throw "Missing mask $name"}
    $cache[$name]=@([regex]::Matches($m.Groups[1].Value,'0x([0-9A-F]{2})') | ForEach-Object {[Convert]::ToInt32($_.Groups[1].Value,16)})
  }
  return ,$cache[$name]
}
function RGB565([int]$c) {
  return [Drawing.Color]::FromArgb([int](($c -shr 11 -band 31)*255/31),[int](($c -shr 5 -band 63)*255/63),[int](($c -band 31)*255/31))
}
$bg=RGB565 0x0000; $surface=RGB565 0x1082; $coral=RGB565 0xFF43; $pale=RGB565 0xFF43
$background=[Drawing.Bitmap]::new((Join-Path $root 'assets/background-preview.png'))
$bmp=[Drawing.Bitmap]::new(720,240)
$g=[Drawing.Graphics]::FromImage($bmp);$g.Clear([Drawing.Color]::Black)
$offset=0
function Draw-Mask($bytes,$w,$h,$x,$y,$fg,$back,$start=0,$rotate=$false) {
  for($row=0;$row -lt $h;$row++){for($col=0;$col -lt $w;$col++){
    $i=$row*$w+$col; $b=$bytes[$start+[int][Math]::Floor($i/2)]
    $a=if($i%2 -eq 0){$b -shr 4}else{$b -band 15}
    $xx=$x+$(if($rotate){$w-1-$col}else{$col})
    $yy=$y+$(if($rotate){$h-1-$row}else{$row})
    if($a -eq 0 -or ($xx-120)*($xx-120)+($yy-120)*($yy-120) -gt 120*120){continue}
    $under=$bmp.GetPixel($offset+$xx,$yy)
    $bmp.SetPixel($offset+$xx,$yy,[Drawing.Color]::FromArgb(
      [int](($fg.R*$a+$under.R*(15-$a))/15),[int](($fg.G*$a+$under.G*(15-$a))/15),[int](($fg.B*$a+$under.B*(15-$a))/15)))
  }}
}
function Label($caption,$height,$cx,$y,$back=$bg,$rotate=$false) {
  $pattern='\{"'+[regex]::Escape($caption)+'",(\d+),'+$height+',label(\d+)\}'
  $m=[regex]::Match($text,$pattern)
  if(!$m.Success){throw "Missing label $caption/$height"}
  $w=[int]$m.Groups[1].Value
  Draw-Mask (Get-Mask $text ('label'+$m.Groups[2].Value)) $w $height ([int][Math]::Floor($cx-$w/2)) $y $pale $back 0 $rotate
}
function Button($caption,$x,$y,$w,$h,$selected=$false) {
  $brush=[Drawing.SolidBrush]::new($(if($selected){RGB565 0x39C7}else{$surface}))
  $g.FillRectangle($brush,$offset+$x,$y,$w,$h)
  Label $caption 16 ($x+$w/2) ($y+[int](($h-16)/2)) $brush.Color
  $brush.Dispose()
}
foreach($page in 0..2){
  $offset=$page*240
  $brush=[Drawing.SolidBrush]::new($bg);$g.FillEllipse($brush,$offset,0,240,240);$brush.Dispose()
  if($page -eq 0){
    for($y=0;$y -lt 240;$y++){for($x=0;$x -lt 240;$x++){
      if(($x-120)*($x-120)+($y-120)*($y-120) -le 14400){$bmp.SetPixel($x,$y,$background.GetPixel($x,$y))}
    }}
    $pen=[Drawing.Pen]::new((RGB565 0x39C7));$g.DrawLine($pen,$offset+16,120,$offset+224,120);$pen.Dispose()
    Label 'P1' 12 120 17 $bg $true;Label 'P2' 12 120 211
    foreach($cy in @(60,180)){
      $data=Get-Mask $digits 'digits40';$w=34;$h=40;$x=85
      foreach($d in @(3,4)){
        $drawX=if($cy -eq 60){206-$x}else{$x}
        Draw-Mask $data $w $h $drawX ($cy-20) $coral $bg ($d*680) ($cy -eq 60)
        $x+=36
      }
      $pen=[Drawing.Pen]::new($pale,3)
      $g.DrawLine($pen,$offset+34,$cy,$offset+46,$cy)
      $g.DrawLine($pen,$offset+194,$cy,$offset+206,$cy)
      $g.DrawLine($pen,$offset+200,$cy-6,$offset+200,$cy+6);$pen.Dispose()
    }
    foreach($spec in @(@('settings',96),@('refresh',144))){
      Draw-Mask (Get-Mask $icons 'plate') 36 36 ($spec[1]-18) 102 $surface $bg
      Draw-Mask (Get-Mask $icons $spec[0]) 24 24 ($spec[1]-12) 108 $pale $surface
    }
  } elseif($page -eq 1){
    Label 'CONFIGURACOES' 16 120 32
    Button 'PV JOGADOR 1' 25 55 190 35
    Button 'PV JOGADOR 2' 25 95 190 35
    Button 'JOGO: SWU' 25 135 190 35
    Button 'VOLTAR' 46 185 100 32;Button '>' 154 185 38 32
  } else {
    Label 'JOGADORES' 16 120 30
    Button '1' 46 65 64 44;Button '2' 130 65 64 44 $true
    Button '3' 46 125 64 44;Button '4' 130 125 64 44
    Button 'VOLTAR' 60 185 120 35
  }
}
$bmp.Save((Join-Path $PSScriptRoot 'players-preview.png'));$g.Dispose();$bmp.Dispose();$background.Dispose()
