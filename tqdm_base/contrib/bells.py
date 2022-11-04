"""
Even more features than `tqdm_base.auto` (all the bells & whistles):

- `tqdm_base.auto`
- `tqdm_base.tqdm_base.pandas`
- `tqdm_base.contrib.telegram`
    + uses `${TQDM_TELEGRAM_TOKEN}` and `${TQDM_TELEGRAM_CHAT_ID}`
- `tqdm_base.contrib.discord`
    + uses `${TQDM_DISCORD_TOKEN}` and `${TQDM_DISCORD_CHANNEL_ID}`
"""
__all__ = ['tqdm', 'trange']
import warnings
from os import getenv

if getenv("TQDM_SLACK_TOKEN") and getenv("TQDM_SLACK_CHANNEL"):
    from .slack import tqdm, trange
elif getenv("TQDM_TELEGRAM_TOKEN") and getenv("TQDM_TELEGRAM_CHAT_ID"):
    from .telegram import tqdm, trange
elif getenv("TQDM_DISCORD_TOKEN") and getenv("TQDM_DISCORD_CHANNEL_ID"):
    from .discord import tqdm, trange
else:
    from ..auto import tqdm, trange

with warnings.catch_warnings():
    warnings.simplefilter("ignore", category=FutureWarning)
    tqdm.pandas()
