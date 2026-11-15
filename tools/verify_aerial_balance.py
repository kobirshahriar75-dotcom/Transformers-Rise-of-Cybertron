from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

game = read("src/core/Game.cpp")
arena = read("src/world/Arena.cpp")
enemy = read("src/entities/Enemy.cpp")
player = read("src/entities/Player.cpp")
boss = read("src/entities/Boss.cpp")

checks = {
    "Jetpack enabled in Levels 7 and 8": "(levelNumber == 7 || levelNumber == 8)" in game,
    "Level 7 aerial spawn altitude": "(levelNumber == 7) ? 380.0f : 300.0f" in game,
    "Level 7 wave count scales 3/4/5/6": "(levelSevenTraining ? 2 : 3) + waveNum" in arena,
    "Level 7 seeker damage is 10": "levelSevenTraining ? 10.0f : 14.0f" in arena,
    "Level 7 seeker interval is 3 seconds": "levelSevenTraining ? 3.0f : 2.2f" in arena,
    "Level 7 seeker projectile speed is 520": "levelSevenTraining ? 520.0f : 650.0f" in arena,
    "Enemy AI uses configured aerial cooldown": "m_attackCooldown = m_aerialAttackInterval;" in enemy,
    "Enemy AI uses configured aerial projectile speed": "m_facing * m_aerialProjectileSpeed" in enemy,
    "Enemy AI uses configured aerial damage": "m_aerialProjectileDamage, 2.5f" in enemy,
    "Jetpack primary attack takes precedence": "if (m_jetpackMode || m_mode == PlayerMode::PLASMA_CANNON)" in player,
    "Jetpack primary fires plasma projectile": "ProjectileType::PLAYER_PLASMA_BOLT, 18.0f, 1.8f" in player,
    "Jetpack special attack takes precedence": player.count("if (m_jetpackMode || m_mode == PlayerMode::PLASMA_CANNON)") >= 2,
    "Jetpack melee hitbox disabled": "if (m_jetpackMode || m_mode != PlayerMode::ENERGON_BLADE || !m_isAttacking)" in player,
    "Soundwave salvo remains ranged-only": "if (m_bossType == LevelBossType::SOUNDWAVE || m_state != BossState::EXECUTE_ATTACK_2) return hb;" in boss,
    "Soundwave warning window remains": "m_stateTimer = phaseTwo ? 0.50f : 0.70f;" in boss,
}

failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")
print(f"\n{len(checks) - len(failed)}/{len(checks)} checks passed")
if failed:
    raise SystemExit(1)
