"""Create todos and scripts tables.

Revision ID: 20260825_0001
Revises:
Create Date: 2026-08-25
"""
from typing import Optional, Sequence, Union

from alembic import op
import sqlalchemy as sa


revision: str = "20260825_0001"
down_revision: Optional[str] = None
branch_labels: Optional[Union[str, Sequence[str]]] = None
depends_on: Optional[Union[str, Sequence[str]]] = None


def upgrade() -> None:
    # Column order is significant because the C++ repositories use SELECT * and
    # read values by their numeric positions.
    op.create_table(
        "todos",
        sa.Column("id", sa.Integer(), primary_key=True, autoincrement=True),
        sa.Column("title", sa.Text(), nullable=False),
        sa.Column("description", sa.Text(), nullable=False),
        sa.Column("priority", sa.Integer(), nullable=False),
        sa.Column("completed_at", sa.BigInteger(), nullable=False),
        sa.Column(
            "completed",
            sa.Boolean(),
            nullable=False,
            server_default=sa.false(),
        ),
        sa.CheckConstraint("priority >= 0", name="ck_todos_priority_nonnegative"),
        sa.CheckConstraint("completed IN (0, 1)", name="ck_todos_completed_boolean"),
    )

    op.create_table(
        "scripts",
        sa.Column("id", sa.Integer(), primary_key=True, autoincrement=True),
        sa.Column("name", sa.Text(), nullable=False),
        sa.Column("source", sa.Text(), nullable=False),
        sa.Column("event", sa.Integer(), nullable=False),
        sa.Column(
            "enabled",
            sa.Boolean(),
            nullable=False,
            server_default=sa.true(),
        ),
        sa.CheckConstraint("event >= 0", name="ck_scripts_event_nonnegative"),
        sa.CheckConstraint("enabled IN (0, 1)", name="ck_scripts_enabled_boolean"),
    )
    op.create_index("ix_scripts_event", "scripts", ["event"])


def downgrade() -> None:
    op.drop_index("ix_scripts_event", table_name="scripts")
    op.drop_table("scripts")
    op.drop_table("todos")
