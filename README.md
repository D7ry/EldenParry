
<div align = center>

# Elden Parry

*A mod that adds parrying to **Skyrim**.*


<br>
<br>

<a href = 'https://youtu.be/kLbVoy-hZpI'>
  <img
   src = 'Resources/Showcase.webp'
   width = 400
  />
</a>

<br>
<br>
<br>

## The Problem

Skyrim's bash is overpowered and boring.

Every single bash staggers the enemy, <br>
this turns the late game combat into:

<kbd>  Bash  </kbd>  🠖  <kbd>  Some Swings  </kbd>  🠖  <kbd>  Repeat  </kbd>

This will happen despite all the combat <br>
overhaul mods you may have installed.

<br>
<br>

## This Mod

Light bashes no longer have a hit frame. <br>
This means doing a light bash alone will <br>
not hit and stagger an enemy.

<br>

![No Light Bash Spamming]

<br>

Light bash is now turned into 'parry'.

When you bash while getting hit, you parry <br>
the incoming attack and stagger the attacker.

Successful parries do not cost stamina.

<br>

![Parry Precision]

<br>

Parrying an incoming projectile <br>
deflects it back at the enemy.

<br>

![Magic Projectiles]

<br>

Works for magic projectiles like fireballs, ice spikes, etc. as well.

Projectile parrying is done by hooking the projectile collision <br>
function, not with workarounds like  spawning a new projectile <br>
to shoot back / giving a perk to negate projectile damage.

The projectile that you deflect back is the original projectile.

With the above changes, bashing / parrying is now much <br>
more interesting and dynamic, its no longer a 'no brainer' <br>
but rather a skill you need to practice on and harness.

This won't fundamentally change Skyrim's combat feel, <br>
but it will spice things up a lot, and it feels great in both <br>
1st and 3rd person.

<br>

<a href = 'https://youtu.be/-Vcc2XaTdDQ'>
  <img
   src = 'Resources/Custom%20Sound.webp'
   width = 400
  />
</a>

<br>
<br>

Everything is done through a lightweight **SKSE Plugin** <br>
paired with a **ESL** to store some sound records. 

</div>

<br>
<br>

## Settings

The mod comes with an `.ini` file in which you can tweak <br>
some settings for this mod to better suit your loadout.

You can disable weapon / shield parry or projectile parry, <br>
as well as tweak the visual feedback parrying does, <br>
including a God-Of-War like slow time effect.

By default, successful parrying won't cost you any stamina.

If you use mods that speed up stamina recovery however, <br>
you will want to disable this option for a balanced experience.

If you feel parrying is too easy, you can also tweak <br>
the start and ending time of your parrying window.

<br>
<br>

## Compatibility

-   Usable with every single combat mods / behavior mods.

-   Incompatible with **[Better God Modes]**.

-   If you use **Ersh's Precision Beta**, make sure you update the <br>
    mod to `0.4+`, otherwise you will have problems parrying.

-   There will be an update on **Valhalla Combat Beta** soon <br>
    that allows Elden Parry to do extra stun damage to NPCs.


<br>
<br>

## Credits

-   **Gigabrain** clib people

-   **Ersh** for helping me with a lot of Havok stuff <br>
    (without which projectile deflection won't be <br>
    possible) and precision API

-   **Fenix** for helping me RE the playsound function

-   **MaxSu** for his Simple Block Sparks source code

-   **Elden Ring** for the inspiration to turn bash into parry

-   **绝伦少年** for the cover image

<br>


<!----------------------------------------------------------------------------->

[Better God Modes]: https://www.nexusmods.com/skyrimspecialedition/mods/69839

[No Light Bash Spamming]: Resources/No%20Light%20Bash%20Spamming.gif
[Magic Projectiles]: Resources/Magic%20Projectiles.gif
[Parry Precision]: Resources/Parry%20Precision.gif
